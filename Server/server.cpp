#include "stdafx.h"
#include "ServerENet.h"
#include <Windows.h>

#include <map>
#include "message.h"

using namespace ENet;

#define PORT 1234
#define MAX_CONNECTIONS_PER_SECOND 5

#define RELIABLE_MESSAGE true
#define UNRELIABLE_MESSAGE false

#define BACKGROUND_WIDTH 2048
#define BACKGROUND_HEIGHT 1020

#define NUMBER_OF_PICKEABLES 17
#define CIRCLE_IMAGE_RADIUS 800

#define INITIAL_PICKEABLE_SCALE 0.015f
#define INITIAL_PLAYER_SCALE 0.025f
#define INITIAL_PLAYER_MOVEMENT_SPEED 200

#define SCALE_PROGRESSION 0.08f
#define MOVEMENT_SPEED_PROGRESSION 0.04f
#define MIN_MOVEMENT_SPEED 60

// --------------------------------------------------
// Global variables
// --------------------------------------------------
CServerENet* pServer;
std::map<int, CPeerENet*> clientList;

int clientIDCounter = 0;
int pickeableIDCounter = 0;

Message clientMessage;
std::vector<Player*> playersSnapshot;
std::vector<Pickeable*> pickeablesSnapshot;

// --------------------------------------------------
// Functions
// --------------------------------------------------
void ReceiveData(CPacketENet* packet);
void AddClient(CPeerENet* peer);
void RemoveClient(CPeerENet* peer);
void BroadcastPlayersSnapshot();
void BroadcastPickeablesSnapshot();
void ManageMovementRequest(CBuffer data);
void CheckCollisionWithPlayers(Player* player);
void CheckCollisionWithPickeable(Player* player);
void ApplyPlayerProgression(Player* player);
int GetRandomValue(int min, int max);

int main(int argc, _TCHAR* argv[])
{
    pServer = new CServerENet();
	CBuffer dataBuffer;

	// Create pickeables
	for (int i = 0; i < NUMBER_OF_PICKEABLES; i++)
	{
		pickeablesSnapshot.push_back(new Pickeable(pickeableIDCounter, GetRandomValue(0, BACKGROUND_WIDTH - 400), GetRandomValue(0, BACKGROUND_HEIGHT - 400), INITIAL_PICKEABLE_SCALE, (CIRCLE_IMAGE_RADIUS * INITIAL_PICKEABLE_SCALE) / 2, 0, 200, 0));
		pickeableIDCounter += 1;
	}

    if (pServer->Init(PORT, MAX_CONNECTIONS_PER_SECOND))
    {
        while (true)
        {
			std::vector<CPacketENet*> incomingPackets;
            pServer->Service(incomingPackets, 0);
            Sleep(17);

			for (auto incomingPacket = incomingPackets.begin(); incomingPacket != incomingPackets.end(); ++incomingPacket)
			{
				switch ((*incomingPacket)->GetType())
				{
				case CONNECT:
					AddClient((*incomingPacket)->GetPeer());
					BroadcastPlayersSnapshot();
					BroadcastPickeablesSnapshot();
					break;
				case DATA:
					ReceiveData(*incomingPacket);
					break;
				case DISCONNECT:
					printf("Disconnected client\n");
					RemoveClient((*incomingPacket)->GetPeer());
					break;
				}
			}
        }
    }
    else
    {
        printf("Server could not be initialized.\n");
    }

    return 0;
}

void ReceiveData(CPacketENet* packet)
{
	CBuffer data;

	data.Write(packet->GetData(), packet->GetDataLength());
	data.GotoStart();

	clientMessage.Deserialize(data);

	switch (clientMessage.m_type)
	{
	case MESSAGE_MOVE:
		ManageMovementRequest(data);
		break;
	}
}

void AddClient(CPeerENet* peer)
{
	CBuffer data;

	clientList.insert(std::pair<int, CPeerENet*>(clientIDCounter, peer));
	playersSnapshot.push_back(new Player(clientIDCounter, GetRandomValue(0, BACKGROUND_WIDTH - 400), GetRandomValue(0, BACKGROUND_HEIGHT - 400), INITIAL_PLAYER_MOVEMENT_SPEED, INITIAL_PLAYER_SCALE, (CIRCLE_IMAGE_RADIUS * INITIAL_PLAYER_SCALE) / 2, GetRandomValue(40, 255), GetRandomValue(40, 255), GetRandomValue(40, 255)));

	MessageAssignClientID assignClientIDMessage;
	assignClientIDMessage.m_ID = clientIDCounter;
	assignClientIDMessage.Serialize(data);

	pServer->SendData(peer, data.GetBytes(), data.GetSize(), 0, RELIABLE_MESSAGE);

	clientIDCounter += 1;
}

void RemoveClient(CPeerENet* peer)
{
	int clientIdToRemove = -1;

	for (auto client = clientList.begin(); client != clientList.end(); ++client)
	{
		if ((*client).second == peer)
		{
			clientIdToRemove = (*client).first;
		}
	}

	if (clientIdToRemove != -1)
	{
		// Remove from client list
		clientList.erase(clientIdToRemove);
		
		// Remove from players snapshot
		for (auto player = playersSnapshot.begin(); player != playersSnapshot.end();)
		{
			if ((*player)->m_ID == clientIdToRemove)
			{
				player = playersSnapshot.erase(player);
			}
			else
			{
				++player;
			}
		}

		// Send message to clients
		CBuffer dataBuffer;
		MessageDisconnectedClient disconnectedClientMessage;

		disconnectedClientMessage.m_ID = clientIdToRemove;
		disconnectedClientMessage.Serialize(dataBuffer);

		for (auto client = clientList.begin(); client != clientList.end(); ++client)
		{
			pServer->SendData((*client).second, dataBuffer.GetBytes(), dataBuffer.GetSize(), 0, UNRELIABLE_MESSAGE);
		}

		BroadcastPlayersSnapshot();
	}
}

void BroadcastPlayersSnapshot()
{
	CBuffer dataBuffer;
	MessagePlayersSnapshot snapshotMessage;

	snapshotMessage.m_playersSnapshot = playersSnapshot;
	snapshotMessage.Serialize(dataBuffer);

	for (auto client = clientList.begin(); client != clientList.end(); ++client)
	{
		pServer->SendData((*client).second, dataBuffer.GetBytes(), dataBuffer.GetSize(), 0, UNRELIABLE_MESSAGE);
	}
}

void BroadcastPickeablesSnapshot()
{
	CBuffer dataBuffer;
	MessagePickeablesSnapshot snapshotMessage;

	snapshotMessage.m_pickeablesSnapshot = pickeablesSnapshot;
	snapshotMessage.Serialize(dataBuffer);

	for (auto client = clientList.begin(); client != clientList.end(); ++client)
	{
		pServer->SendData((*client).second, dataBuffer.GetBytes(), dataBuffer.GetSize(), 0, UNRELIABLE_MESSAGE);
	}
}

void ManageMovementRequest(CBuffer data)
{
	MessageMove moveMessage;
	moveMessage.Deserialize(data);

	int nextMovement = 0;

	for (auto player = playersSnapshot.begin(); player != playersSnapshot.end(); ++player)
	{
		if ((*player)->m_ID == moveMessage.m_clientID)
		{
			nextMovement = (*player)->m_movementSpeed * moveMessage.m_elapsedTime;

			// Diagonal
			if (moveMessage.m_isMovingUp && moveMessage.m_isMovingLeft || moveMessage.m_isMovingUp && moveMessage.m_isMovingRight ||
				moveMessage.m_isMovingDown && moveMessage.m_isMovingLeft || moveMessage.m_isMovingDown && moveMessage.m_isMovingRight)
			{
				nextMovement -= nextMovement / 3;
			}

			// Up/down
			if (moveMessage.m_isMovingUp)
			{
				if ((*player)->m_positionY - (*player)->m_radius - nextMovement > 0)
				{
					(*player)->m_positionY -= nextMovement;
				}
			}
			else if (moveMessage.m_isMovingDown)
			{
				if ((*player)->m_positionY + (*player)->m_radius + nextMovement < BACKGROUND_HEIGHT)
				{
					(*player)->m_positionY += nextMovement;
				}
			}

			// Left/right
			if (moveMessage.m_isMovingLeft)
			{
				if ((*player)->m_positionX - (*player)->m_radius - nextMovement > 0)
				{
					(*player)->m_positionX -= nextMovement;
				}
			}
			else if (moveMessage.m_isMovingRight)
			{
				if ((*player)->m_positionX + (*player)->m_radius + nextMovement < BACKGROUND_WIDTH)
				{
					(*player)->m_positionX += nextMovement;
				}
			}
		}

		// Check collisions with players and pickeables
		CheckCollisionWithPlayers(*player);
		CheckCollisionWithPickeable(*player);
	}

	BroadcastPlayersSnapshot();
}

void CheckCollisionWithPlayers(Player* player)
{
	for (auto playerIterator = playersSnapshot.begin(); playerIterator != playersSnapshot.end(); ++playerIterator)
	{
		if ((*playerIterator)->m_ID != player->m_ID)
		{
			double distance = sqrt(pow((*playerIterator)->m_positionX - player->m_positionX, 2) + pow((*playerIterator)->m_positionY - player->m_positionY, 2));
			
			if (distance <= (*playerIterator)->m_radius + player->m_radius)
			{
				if (player->m_radius < (*playerIterator)->m_radius)
				{
					player->m_positionX = GetRandomValue(0, BACKGROUND_WIDTH - 400);
					player->m_positionY = GetRandomValue(0, BACKGROUND_HEIGHT - 400);
					player->m_scale = INITIAL_PLAYER_SCALE;
					player->m_radius = (CIRCLE_IMAGE_RADIUS * INITIAL_PLAYER_SCALE) / 2;
					player->m_movementSpeed = INITIAL_PLAYER_MOVEMENT_SPEED;

					ApplyPlayerProgression(*playerIterator);
				}
				else if (player->m_radius > (*playerIterator)->m_radius)
				{
					(*playerIterator)->m_positionX = GetRandomValue(0, BACKGROUND_WIDTH - 400);
					(*playerIterator)->m_positionY = GetRandomValue(0, BACKGROUND_HEIGHT - 400);
					(*playerIterator)->m_scale = INITIAL_PLAYER_SCALE;
					(*playerIterator)->m_radius = (CIRCLE_IMAGE_RADIUS * INITIAL_PLAYER_SCALE) / 2;
					(*playerIterator)->m_movementSpeed = INITIAL_PLAYER_MOVEMENT_SPEED;

					ApplyPlayerProgression(player);
				}
			}
		}
	}
}

void CheckCollisionWithPickeable(Player* player)
{
	bool collisionDetected = false;

	for (auto pickeable = pickeablesSnapshot.begin(); pickeable != pickeablesSnapshot.end(); ++pickeable)
	{
		double distance = sqrt(pow((*pickeable)->m_positionX - player->m_positionX, 2) + pow((*pickeable)->m_positionY - player->m_positionY, 2));

		if (distance <= (*pickeable)->m_radius + player->m_radius)
		{
			collisionDetected = true;

			ApplyPlayerProgression(player);

			(*pickeable)->m_positionX = GetRandomValue(0, BACKGROUND_WIDTH);
			(*pickeable)->m_positionY = GetRandomValue(0, BACKGROUND_HEIGHT);
		}
	}

	if (collisionDetected)
	{
		BroadcastPickeablesSnapshot();
	}
}

void ApplyPlayerProgression(Player* player)
{
	player->m_scale += player->m_scale * SCALE_PROGRESSION;
	player->m_radius = (CIRCLE_IMAGE_RADIUS * player->m_scale) / 2;

	if (player->m_movementSpeed - (player->m_movementSpeed * MOVEMENT_SPEED_PROGRESSION) >= MIN_MOVEMENT_SPEED)
	{
		player->m_movementSpeed -= (player->m_movementSpeed * MOVEMENT_SPEED_PROGRESSION);
	}
	else
	{
		player->m_movementSpeed = MIN_MOVEMENT_SPEED;
	}
}

int GetRandomValue(int min, int max)
{
	return min + (rand() % (max - min + 1));
}
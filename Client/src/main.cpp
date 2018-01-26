//#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

#include "../include/u-gine.h"
#include "ClientENet.h"
#include <Windows.h>

#include <map>
#include "message.h"

using namespace ENet;

#define SERVER_ADDRESS "127.0.0.1"
#define SERVER_PORT 1234
#define SERVER_CHANNELS 2

#define RELIABLE_MESSAGE true
#define UNRELIABLE_MESSAGE false

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

// --------------------------------------------------
// Global variables
// --------------------------------------------------
CClienteENet* pClient;
int clientID;

Message serverMessage;
MessageMove moveMessage;

std::vector<Player*> playersSnapshot;
std::vector<Pickeable*> pickeableSnapshot;

std::map<int, Sprite*> playerSprites;
std::map<int, Sprite*> pickeableSprites;

Image* background;
Image* circleImage;

Scene* scene;

// --------------------------------------------------
// Functions
// --------------------------------------------------
void ReceiveData(CBuffer data);
void AssignID(CBuffer data);
void UpdatePlayersSnapshot(CBuffer data);
void UpdatePickeableSnapshot(CBuffer data);
void RemoveDisconnectedClient(CBuffer data);

int main(int argc, char* argv[])
{
	Screen::Instance().Open(SCREEN_WIDTH, SCREEN_HEIGHT, false);

	background = ResourceManager::Instance().LoadImageUgine("data/background.jpg");
	circleImage = ResourceManager::Instance().LoadImageUgine("data/player.png");
	circleImage->SetMidHandle();

	scene = new Scene(background);
	Camera& camera = scene->GetCamera();
	camera.SetBounds(0, 0, background->GetWidth(), background->GetHeight());

	// Enet
	CBuffer dataBuffer;

	pClient = new CClienteENet();
	pClient->Init();

	CPeerENet* pServer = pClient->Connect(SERVER_ADDRESS, SERVER_PORT, SERVER_CHANNELS);

	// Main loop
	while (Screen::Instance().IsOpened() && !Screen::Instance().KeyPressed(GLFW_KEY_ESC))
	{
		// Enet
		std::vector<CPacketENet*> incomingPackets;
		pClient->Service(incomingPackets, 0);

		for (auto incomingPacket = incomingPackets.begin(); incomingPacket != incomingPackets.end(); ++incomingPacket)
		{
			switch ((*incomingPacket)->GetType())
			{
			case DATA:
				dataBuffer.Clear();
				dataBuffer.Write((*incomingPacket)->GetData(), (*incomingPacket)->GetDataLength());
				dataBuffer.GotoStart();

				ReceiveData(dataBuffer);
				break;
			case DISCONNECT:
				printf("Server disconnected\n");
				return 0;
			}
		}

		// Move up/down
		if (Screen::Instance().KeyPressed(GLFW_KEY_UP))
		{
			//positionY -= movementSpeed * Screen::Instance().ElapsedTime();
			moveMessage.m_isMovingUp = true;
			moveMessage.m_elapsedTime = Screen::Instance().ElapsedTime();

			moveMessage.Serialize(dataBuffer);
			pClient->SendData(pServer, dataBuffer.GetBytes(), dataBuffer.GetSize(), 0, RELIABLE_MESSAGE);
		}
		else if (Screen::Instance().KeyPressed(GLFW_KEY_DOWN))
		{
			//positionY += movementSpeed * Screen::Instance().ElapsedTime();
			moveMessage.m_isMovingDown = true;
			moveMessage.m_elapsedTime = Screen::Instance().ElapsedTime();

			moveMessage.Serialize(dataBuffer);
			pClient->SendData(pServer, dataBuffer.GetBytes(), dataBuffer.GetSize(), 0, RELIABLE_MESSAGE);
		}

		// Move left/right
		if (Screen::Instance().KeyPressed(GLFW_KEY_LEFT))
		{
			//positionX -= movementSpeed * Screen::Instance().ElapsedTime();
			moveMessage.m_isMovingLeft = true;
			moveMessage.m_elapsedTime = Screen::Instance().ElapsedTime();

			moveMessage.Serialize(dataBuffer);
			pClient->SendData(pServer, dataBuffer.GetBytes(), dataBuffer.GetSize(), 0, RELIABLE_MESSAGE);
		}
		else if (Screen::Instance().KeyPressed(GLFW_KEY_RIGHT))
		{
			//positionX += movementSpeed * Screen::Instance().ElapsedTime();
			moveMessage.m_isMovingRight = true;
			moveMessage.m_elapsedTime = Screen::Instance().ElapsedTime();

			moveMessage.Serialize(dataBuffer);
			pClient->SendData(pServer, dataBuffer.GetBytes(), dataBuffer.GetSize(), 0, RELIABLE_MESSAGE);
		}

		scene->Update(Screen::Instance().ElapsedTime());
		scene->Render();

		// Render pickeables
		for (auto pickeableSprite = pickeableSprites.begin(); pickeableSprite != pickeableSprites.end(); ++pickeableSprite)
		{
			pickeableSprite->second->Render();
		}

		// Render players
		for (auto playerSprite = playerSprites.begin(); playerSprite != playerSprites.end(); ++playerSprite)
		{
			playerSprite->second->Render();
		}

		Screen::Instance().Refresh();

		moveMessage.Reset();

		Sleep(17);
	}

	pClient->Disconnect(pServer);

	ResourceManager::Instance().FreeResources();

	return 0;
}

void ReceiveData(CBuffer data)
{
	serverMessage.Deserialize(data);

	switch (serverMessage.m_type)
	{
	case MESSAGE_ASSIGN_CLIENT_ID:
		AssignID(data);
		break;
	case MESSAGE_PLAYERS_SNAPSHOT:
		UpdatePlayersSnapshot(data);
		break;
	case MESSAGE_PICKEABLES_SNAPSHOT:
		UpdatePickeableSnapshot(data);
		break;
	case MESSAGE_CLIENT_DISCONNECTED:
		RemoveDisconnectedClient(data);
		break;
	}
}

void AssignID(CBuffer data)
{
	MessageAssignClientID assignClientIDMessage;
	assignClientIDMessage.Deserialize(data);
	clientID = assignClientIDMessage.m_ID;

	moveMessage.m_clientID = clientID;
}

void UpdatePlayersSnapshot(CBuffer data)
{
	MessagePlayersSnapshot snapshotMessage;
	snapshotMessage.Deserialize(data);

	playersSnapshot = snapshotMessage.m_playersSnapshot;

	for (auto player = playersSnapshot.begin(); player != playersSnapshot.end(); ++player)
	{
		if (playerSprites.find((*player)->m_ID) == playerSprites.end())
		{
			// Player doesnt exist in playerSprites
			Sprite* playerSprite = new Sprite(circleImage);

			playerSprite->SetScale((*player)->m_scale, (*player)->m_scale);
			playerSprite->SetRadius((*player)->m_radius);
			playerSprite->SetColor((*player)->m_r, (*player)->m_g, (*player)->m_b, 255);
			playerSprite->SetPosition((*player)->m_positionX, (*player)->m_positionY);

			if ((*player)->m_ID == clientID)
			{
				scene->GetCamera().FollowSprite(playerSprite);
			}

			playerSprites.insert(std::pair<int, Sprite*>((*player)->m_ID, playerSprite));
		}
		else
		{
			// Player exists in playerSprites
			playerSprites[(*player)->m_ID]->SetScale((*player)->m_scale, (*player)->m_scale);
			playerSprites[(*player)->m_ID]->SetRadius((*player)->m_radius);
			playerSprites[(*player)->m_ID]->SetPosition((*player)->m_positionX, (*player)->m_positionY);
		}
	}
}

void UpdatePickeableSnapshot(CBuffer data)
{
	MessagePickeablesSnapshot snapshotMessage;
	snapshotMessage.Deserialize(data);

	pickeableSnapshot = snapshotMessage.m_pickeablesSnapshot;

	for (auto pickeable = pickeableSnapshot.begin(); pickeable != pickeableSnapshot.end(); ++pickeable)
	{
		if (pickeableSprites.find((*pickeable)->m_ID) == pickeableSprites.end())
		{
			// Pickeable doesnt exist in pickeableSprites
			Sprite* pickeableSprite = new Sprite(circleImage);

			pickeableSprite->SetScale((*pickeable)->m_scale, (*pickeable)->m_scale);
			pickeableSprite->SetRadius((*pickeable)->m_scale / 2);
			pickeableSprite->SetColor((*pickeable)->m_r, (*pickeable)->m_g, (*pickeable)->m_b, 255);
			pickeableSprite->SetPosition((*pickeable)->m_positionX, (*pickeable)->m_positionY);

			pickeableSprites.insert(std::pair<int, Sprite*>((*pickeable)->m_ID, pickeableSprite));
		}
		else
		{
			// Pickeable exists in pickeableSprites
			pickeableSprites[(*pickeable)->m_ID]->SetPosition((*pickeable)->m_positionX, (*pickeable)->m_positionY);
		}
	}
}

void RemoveDisconnectedClient(CBuffer data)
{
	MessageDisconnectedClient disconnectedClientMessage;
	disconnectedClientMessage.Deserialize(data);

	playerSprites.erase(disconnectedClientMessage.m_ID);
}
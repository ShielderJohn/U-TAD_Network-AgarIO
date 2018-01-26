#pragma once

#include <vector>
#include "../EnetWrapper/Buffer.h"
#include "entity.h"

enum MessageType {
	MESSAGE_ASSIGN_CLIENT_ID,
	MESSAGE_MOVE,
	MESSAGE_PLAYERS_SNAPSHOT,
	MESSAGE_PICKEABLES_SNAPSHOT,
	MESSAGE_CLIENT_DISCONNECTED
};

// Base class
struct Message
{
	MessageType m_type;

	Message()
	{

	}

	virtual ~Message()
	{

	}

	void Serialize(CBuffer& outputBuffer)
	{
		outputBuffer.Clear();
		outputBuffer.GotoStart();

		outputBuffer.Write(&m_type, sizeof(MessageType));

		outputBuffer.GotoStart();
	}

	void Deserialize(CBuffer& inputBuffer)
	{
		inputBuffer.GotoStart();

		inputBuffer.Read(&m_type, sizeof(MessageType));
	}
};

// Assign client ID message
struct MessageAssignClientID : public Message
{
	int m_ID;

	MessageAssignClientID()
	{
		m_type = MESSAGE_ASSIGN_CLIENT_ID;
		m_ID = -1;
	}

	virtual ~MessageAssignClientID()
	{

	}

	void Serialize(CBuffer& outputBuffer)
	{
		outputBuffer.Clear();
		outputBuffer.GotoStart();

		outputBuffer.Write(&m_type, sizeof(MessageType));
		outputBuffer.Write(&m_ID, sizeof(int));

		outputBuffer.GotoStart();
	}

	void Deserialize(CBuffer& inputBuffer)
	{
		inputBuffer.GotoStart();

		inputBuffer.Read(&m_type, sizeof(MessageType));
		inputBuffer.Read(&m_ID, sizeof(int));
	}
};

// Move message
struct MessageMove : public Message
{
	int m_clientID;

	bool m_isMovingUp;
	bool m_isMovingDown;
	bool m_isMovingLeft;
	bool m_isMovingRight;

	double m_elapsedTime;

	MessageMove()
	{
		m_type = MESSAGE_MOVE;

		m_clientID = -1;

		m_isMovingUp = false;
		m_isMovingDown = false;
		m_isMovingLeft = false;
		m_isMovingRight = false;

		m_elapsedTime = 0;
	}

	virtual ~MessageMove()
	{

	}

	void Serialize(CBuffer& outputBuffer)
	{
		outputBuffer.Clear();
		outputBuffer.GotoStart();

		outputBuffer.Write(&m_type, sizeof(MessageType));

		outputBuffer.Write(&m_clientID, sizeof(int));

		outputBuffer.Write(&m_isMovingUp, sizeof(bool));
		outputBuffer.Write(&m_isMovingDown, sizeof(bool));
		outputBuffer.Write(&m_isMovingLeft, sizeof(bool));
		outputBuffer.Write(&m_isMovingRight, sizeof(bool));

		outputBuffer.Write(&m_elapsedTime, sizeof(double));

		outputBuffer.GotoStart();
	}

	void Deserialize(CBuffer& inputBuffer)
	{
		inputBuffer.GotoStart();

		inputBuffer.Read(&m_type, sizeof(MessageType));

		inputBuffer.Read(&m_clientID, sizeof(int));

		inputBuffer.Read(&m_isMovingUp, sizeof(bool));
		inputBuffer.Read(&m_isMovingDown, sizeof(bool));
		inputBuffer.Read(&m_isMovingLeft, sizeof(bool));
		inputBuffer.Read(&m_isMovingRight, sizeof(bool));

		inputBuffer.Read(&m_elapsedTime, sizeof(double));
	}

	void Reset()
	{
		m_isMovingUp = false;
		m_isMovingDown = false;
		m_isMovingLeft = false;
		m_isMovingRight = false;
	}
};

// Players Snapshot message
struct MessagePlayersSnapshot : public Message
{
	std::vector<Player*> m_playersSnapshot;

	MessagePlayersSnapshot()
	{
		m_type = MESSAGE_PLAYERS_SNAPSHOT;
	}

	virtual ~MessagePlayersSnapshot()
	{

	}

	void Serialize(CBuffer& outputBuffer)
	{
		outputBuffer.Clear();
		outputBuffer.GotoStart();

		outputBuffer.Write(&m_type, sizeof(MessageType));

		for (auto player = m_playersSnapshot.begin(); player != m_playersSnapshot.end(); ++player)
		{
			outputBuffer.Write(&(*player)->m_ID, sizeof(int));
			outputBuffer.Write(&(*player)->m_positionX, sizeof(int));
			outputBuffer.Write(&(*player)->m_positionY, sizeof(int));
			outputBuffer.Write(&(*player)->m_movementSpeed, sizeof(int));
			outputBuffer.Write(&(*player)->m_scale, sizeof(double));
			outputBuffer.Write(&(*player)->m_radius, sizeof(double));

			outputBuffer.Write(&(*player)->m_r, sizeof(unsigned char));
			outputBuffer.Write(&(*player)->m_g, sizeof(unsigned char));
			outputBuffer.Write(&(*player)->m_b, sizeof(unsigned char));
		}

		outputBuffer.GotoStart();
	}

	void Deserialize(CBuffer& inputBuffer)
	{
		inputBuffer.GotoStart();
		inputBuffer.Read(&m_type, sizeof(MessageType));

		unsigned bufferSize = inputBuffer.GetSize();
		unsigned iterator = sizeof(MessageType);

		while (iterator < bufferSize)
		{
			Player* player = new Player();

			inputBuffer.Read(&player->m_ID, sizeof(int));
			inputBuffer.Read(&player->m_positionX, sizeof(int));
			inputBuffer.Read(&player->m_positionY, sizeof(int));
			inputBuffer.Read(&player->m_movementSpeed, sizeof(int));
			inputBuffer.Read(&player->m_scale, sizeof(double));
			inputBuffer.Read(&player->m_radius, sizeof(double));

			inputBuffer.Read(&player->m_r, sizeof(unsigned char));
			inputBuffer.Read(&player->m_g, sizeof(unsigned char));
			inputBuffer.Read(&player->m_b, sizeof(unsigned char));

			m_playersSnapshot.push_back(player);

			iterator += (sizeof(int) * 4) + (sizeof(double) * 2) + sizeof(unsigned char) * 3;
		}
	}
};

// Pickeable Snapshot message
struct MessagePickeablesSnapshot : public Message
{
	std::vector<Pickeable*> m_pickeablesSnapshot;

	MessagePickeablesSnapshot()
	{
		m_type = MESSAGE_PICKEABLES_SNAPSHOT;
	}

	virtual ~MessagePickeablesSnapshot()
	{

	}

	void Serialize(CBuffer& outputBuffer)
	{
		outputBuffer.Clear();
		outputBuffer.GotoStart();

		outputBuffer.Write(&m_type, sizeof(MessageType));

		for (auto pickeable = m_pickeablesSnapshot.begin(); pickeable != m_pickeablesSnapshot.end(); ++pickeable)
		{
			outputBuffer.Write(&(*pickeable)->m_ID, sizeof(int));
			outputBuffer.Write(&(*pickeable)->m_positionX, sizeof(int));
			outputBuffer.Write(&(*pickeable)->m_positionY, sizeof(int));
			outputBuffer.Write(&(*pickeable)->m_scale, sizeof(double));
			outputBuffer.Write(&(*pickeable)->m_radius, sizeof(double));

			outputBuffer.Write(&(*pickeable)->m_r, sizeof(unsigned char));
			outputBuffer.Write(&(*pickeable)->m_g, sizeof(unsigned char));
			outputBuffer.Write(&(*pickeable)->m_b, sizeof(unsigned char));
		}

		outputBuffer.GotoStart();
	}

	void Deserialize(CBuffer& inputBuffer)
	{
		inputBuffer.GotoStart();
		inputBuffer.Read(&m_type, sizeof(MessageType));

		unsigned bufferSize = inputBuffer.GetSize();
		unsigned iterator = sizeof(MessageType);

		while (iterator < bufferSize)
		{
			Pickeable* pickeable = new Pickeable();

			inputBuffer.Read(&pickeable->m_ID, sizeof(int));
			inputBuffer.Read(&pickeable->m_positionX, sizeof(int));
			inputBuffer.Read(&pickeable->m_positionY, sizeof(int));
			inputBuffer.Read(&pickeable->m_scale, sizeof(double));
			inputBuffer.Read(&pickeable->m_radius, sizeof(double));

			inputBuffer.Read(&pickeable->m_r, sizeof(unsigned char));
			inputBuffer.Read(&pickeable->m_g, sizeof(unsigned char));
			inputBuffer.Read(&pickeable->m_b, sizeof(unsigned char));

			m_pickeablesSnapshot.push_back(pickeable);

			iterator += (sizeof(int) * 3) + (sizeof(double) * 2) + sizeof(unsigned char) * 3;
		}
	}
};

struct MessageDisconnectedClient : public Message
{
	int m_ID;

	MessageDisconnectedClient()
	{
		m_type = MESSAGE_CLIENT_DISCONNECTED;
	}

	virtual ~MessageDisconnectedClient()
	{

	}

	void Serialize(CBuffer& outputBuffer)
	{
		outputBuffer.Clear();
		outputBuffer.GotoStart();

		outputBuffer.Write(&m_type, sizeof(MessageType));

		outputBuffer.Write(&m_ID, sizeof(int));

		outputBuffer.GotoStart();
	}

	void Deserialize(CBuffer& inputBuffer)
	{
		inputBuffer.GotoStart();

		inputBuffer.Read(&m_type, sizeof(MessageType));

		inputBuffer.Read(&m_ID, sizeof(int));
	}
};
#pragma once

struct Entity
{
	int m_ID;
	int m_positionX;
	int m_positionY;
	double m_scale;
	double m_radius;

	unsigned char m_r;
	unsigned char m_g;
	unsigned char m_b;
};

struct Player : public Entity
{
	int m_movementSpeed;

	Player()
	{

	}

	Player(int ID, int positionX, int positionY, int movementSpeed, double scale, double radius, unsigned char r, unsigned char g, unsigned char b)
	{
		m_ID = ID;
		m_positionX = positionX;
		m_positionY = positionY;
		m_movementSpeed = movementSpeed;
		m_scale = scale;
		m_radius = radius;

		m_r = r;
		m_g = g;
		m_b = b;
	}
};

struct Pickeable : public Entity
{
	Pickeable()
	{

	}

	Pickeable(int ID, int positionX, int positionY, double scale, double radius, unsigned int r, unsigned int g, unsigned int b)
	{
		m_ID = ID;
		m_positionX = positionX;
		m_positionY = positionY;
		m_scale = scale;
		m_radius = radius;

		m_r = r;
		m_g = g;
		m_b = b;
	}
};
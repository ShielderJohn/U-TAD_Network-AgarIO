#include "../include/particle.h"


Particle::Particle() : Sprite(NULL) {

}

Particle::Particle(Image* image, double velx, double vely,
	double angularVel, double lifetime, bool autofade) : Sprite(image) {
	velocityx = velx;
	velocityy = vely;
	angularVelocity = angularVel;
	this->lifetime = lifetime;
	initialLifeTime = lifetime;
	this->autofade = autofade;
	affected = false;
}

void Particle::Update(double elapsed) {
	// Move
	SetPosition(GetX() + velocityx * elapsed, GetY() + velocityy * elapsed);

	// Rotate
	SetAngle(GetAngle() + angularVelocity * elapsed);

	// Interpolate
	if (autofade)
		SetColor(GetRed(), GetGreen(), GetBlue(), (lifetime * 255) / initialLifeTime);
		
	lifetime -= elapsed;
}

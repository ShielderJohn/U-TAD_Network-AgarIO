#ifndef UGINE_PARTICLE_H
#define UGINE_PARTICLE_H

#include "sprite.h"


class Particle : public Sprite {
public:
	Particle();
	Particle(Image* image, double velx, double vely, double angularVel, double lifetime, bool autofade);

	virtual double GetLifeTime() const { return lifetime; }
	virtual bool IsAffected() const { return affected; }

	virtual void SetVelocity(double velx, double vely) { velocityx = velx; velocityy = vely; }
	virtual void SetAngularVelocity(double velocity) { angularVelocity = velocity; }
	virtual void SetAffected(bool affected) { this->affected = affected; }

	virtual void Update(double elapsed);
	
private:
	double velocityx, velocityy;
	double angularVelocity;
	double lifetime;
	double initialLifeTime;
	bool autofade;
	bool affected;
};

#endif
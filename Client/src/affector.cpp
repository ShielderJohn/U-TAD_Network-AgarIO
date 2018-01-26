#include <math.h>
#include "../include/particle.h"
#include "../include/affector.h"


Affector::Affector() {

}

Affector::Affector(double minX, double minY, double maxX, double maxY) {
	this->minX = minX;
	this->minY = minY;
	this->maxX = maxX;
	this->maxY = maxY;

	minr = 0;
	ming = 0;
	minb = 0;
	maxr = 0;
	maxg = 0;
	maxb = 0;

	minvelx = 0;
	maxvelx = 0;
	minvely = 0;
	maxvely = 0;

	minangvel = 0;
	maxangvel = 0;
}

void Affector::Update(Particle& particle) {
	double particleX = particle.GetX();
	double particleY = particle.GetY();

	if (particleX >= minX && particleX <= maxX &&
		particleY >= minY && particleY <= maxY &&
		!particle.IsAffected()) {
		double velx = minvelx + fmod(rand(), maxvelx - minvelx + 1);
		double vely = minvely + fmod(rand(), maxvely - minvely + 1);
		double angvel = minangvel + fmod(rand(), maxangvel - minangvel + 1);

		uint8 r = minr + (rand() % (maxr - minr + 1));
		uint8 g = ming + (rand() % (maxg - ming + 1));
		uint8 b = minb + (rand() % (maxb - minb + 1));

		particle.SetVelocity(velx, vely);
		particle.SetAngularVelocity(angvel);
		particle.SetColor(r, g, b, particle.GetAlpha());

		particle.SetAffected(true);
	}
}
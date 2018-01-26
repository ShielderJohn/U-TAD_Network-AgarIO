#ifndef UGINE_AFFECTOR_H
#define UGINE_AFFECTOR_H

#include "types.h"


class Particle;

class Affector {
public:
	Affector();
	Affector(double minX, double minY, double maxX, double maxY);

	void SetRegion(double minX, double minY, double maxX, double maxY) { this->minX = minX; this->minY = minY; this->maxX = maxX; this->maxY = maxY; }
	void SetMinColor(uint8 r, uint8 g, uint8 b) { minr = r; ming = g; minb = b; }
	void SetMaxColor(uint8 r, uint8 g, uint8 b) { maxr = r; maxg = g; maxb = b; }
	void SetVelocityX(double minvelx, double maxvelx) { this->minvelx = minvelx; this->maxvelx = maxvelx; }
	void SetVelocityY(double minvely, double maxvely) { this->minvely = minvely; this->maxvely = maxvely; }
	void SetAngularVelocity(double minangvel, double maxangvel) { this->minangvel = minangvel; this->maxangvel = maxangvel; }

	void Update(Particle& particle);

private:
	double minX, minY;
	double maxX, maxY;
	
	uint8 minr, ming, minb;
	uint8 maxr, maxg, maxb;

	double minvelx, maxvelx;
	double minvely, maxvely;

	double minangvel, maxangvel;
};

#endif
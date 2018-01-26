#ifndef UGINE_PIXELCOLLISION_H
#define UGINE_PIXELCOLLISION_H

#include "collision.h"
#include "collisionmanager.h"


class PixelCollision : public Collision {
public:
	PixelCollision(const CollisionPixelData* pixels, double* px, double* py) { this->pixels = pixels; x = px; y = py; }

	virtual bool DoesCollide(const Collision* other) const { return other->DoesCollide(pixels, *x, *y); }
	virtual bool DoesCollide(double cx, double cy, double cr) const { return CollisionManager::Instance().CircleToPixels(cx, cy, cr, pixels, *x, *y); }
	virtual bool DoesCollide(double rx, double ry, double rw, double rh) const { return CollisionManager::Instance().PixelsToRect(pixels, *x, *y, rx, ry, rw, rh); }
	virtual bool DoesCollide(const CollisionPixelData* pixels, double px, double py) const { return CollisionManager::Instance().PixelsToPixels(this->pixels, *x, *y, pixels, px, py); }

private:
	const CollisionPixelData* pixels;
	double* x;
	double* y;
};

#endif
#include <cmath>
#include "../include/math.h"
#include "../include/collisionpixeldata.h"
#include "../include/collisionmanager.h"



CollisionManager* CollisionManager::manager = 0;

const CollisionManager& CollisionManager::Instance() {
	if (!manager)
		manager = new CollisionManager();
	return *manager;
}

bool CollisionManager::CircleToCircle(double x1, double y1, double r1, double x2, double y2, double r2) const {
	bool returnValue = false;

	if (Distance(x1, y1, x2, y2) <= r1 + r2)
		returnValue = true;

	return returnValue;
}

bool CollisionManager::CircleToPixels(double cx, double cy, double cr, const CollisionPixelData* pixels, double px, double py) const {
	bool returnValue = false;
	double closestPointX, closestPointY;
	double overx, overy, overw, overh;

	if (RectsOverlap(px, py, pixels->GetWidth(), pixels->GetHeight(), cx - cr, cy - cr, cr * 2, cr * 2) ||
		RectsOverlap(cx - cr, cy - cr, cr * 2, cr * 2, px, py, pixels->GetWidth(), pixels->GetHeight())) {
		OverlappingRect(cx - cr, cy - cr, cr * 2, cr * 2, px, py, pixels->GetWidth(), pixels->GetHeight(), &overx, &overy, &overw, &overh);

		for (int y = 0; y < overh; y++) {
			for (int x = 0; x < overw; x++) {
				if (pixels->GetData(fabs(overx - px) + x, fabs(overy - py) + y) == true) {
					ClosestPointToRect(cx, cy, px, py, pixels->GetWidth(), pixels->GetHeight(), &closestPointX, &closestPointY);
					
					if (Distance(cx, cy, closestPointX, closestPointY) <= cr)
						returnValue = true;
				}
			}
		}
	}

	return returnValue;
}

bool CollisionManager::CircleToRect(double cx, double cy, double cr, double rx, double ry, double rw, double rh) const {
	bool returnValue = false;
	double closestPointX, closestPointY;

	ClosestPointToRect(cx, cy, rx, ry, rw, rh, &closestPointX, &closestPointY);

	if (Distance(cx, cy, closestPointX, closestPointY) <= cr)
		returnValue = true;

	return returnValue;
}

bool CollisionManager::PixelsToPixels(const CollisionPixelData* p1, double x1, double y1, const CollisionPixelData* p2, double x2, double y2) const {
	bool returnValue = false;
	double overx, overy, overw, overh;

	if (RectsOverlap(x1, y1, p1->GetWidth(), p1->GetHeight(), x2, y2, p2->GetWidth(), p2->GetHeight())) {
		OverlappingRect(x1, y1, p1->GetWidth(), p1->GetHeight(), x2, y2, p2->GetWidth(), p2->GetHeight(), &overx, &overy, &overw, &overh);

		for (int py = 0; py < overh; py++) {
			for (int px = 0; px < overw; px++) {
				if (p1->GetData(fabs(overx - x1) + px, fabs(overy - y1) + py) == true &&
					p2->GetData(fabs(overx - x2) + px, fabs(overy - y2) + py) == true)
					returnValue = true;
			}
		}
	}

	return returnValue;
}

bool CollisionManager::PixelsToRect(const CollisionPixelData* pixels, double px, double py, double rx, double ry, double rw, double rh) const {
	bool returnValue = false;
	double overx, overy, overw, overh;

	if (RectsOverlap(px, py, pixels->GetWidth(), pixels->GetHeight(), rx, ry, rw, rh) ||
		RectsOverlap(rx, ry, rw, rh, px, py, pixels->GetWidth(), pixels->GetHeight())) {
		OverlappingRect(rx, ry, rw, rh, px, py, pixels->GetWidth(), pixels->GetHeight(), &overx, &overy, &overw, &overh);

		for (int y = 0; y < overh; y++) {
			for (int x = 0; x < overw; x++) {
				if (pixels->GetData(fabs(overx - px) + x, fabs(overy - py) + y) == true)
					returnValue = true;
			}
		}
	}

	return returnValue;
}

bool CollisionManager::RectToRect(double x1, double y1, double w1, double h1, double x2, double y2, double w2, double h2) const {
	bool returnValue = false;

	if (RectsOverlap(x1, y1, w1, h1, x2, y2, w2, h2))
		returnValue = true;

	return returnValue;
}
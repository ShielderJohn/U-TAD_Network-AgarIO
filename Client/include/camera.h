#ifndef UGINE_CAMERA_H
#define UGINE_CAMERA_H

class Sprite;

class Camera {
public:
	Camera();

	virtual void SetPosition(double x, double y);
	virtual void SetX(double x) { this->x = x; }
	virtual void SetY(double y) { this->y = y; }
	virtual double GetX() const { return x; }
	virtual double GetY() const { return y; }

	virtual void SetBounds(double bx0, double by0, double bx1, double by1);
	virtual bool HasBounds() const { return boundx0 != boundx1; }
	virtual double GetMinX() const { return boundx0; }
	virtual double GetMinY() const { return boundy0; }
	virtual double GetMaxX() const { return boundx1; }
	virtual double GetMaxY() const { return boundy1; }

	virtual void FollowSprite(Sprite* sprite);

	virtual void Update();

private:
	double x, y;
	double boundx0, boundy0, boundx1, boundy1;
	Sprite* followingSprite;
};

#endif
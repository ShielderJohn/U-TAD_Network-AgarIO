#include "../include/sprite.h"
#include "../include/screen.h"
#include "../include/camera.h"

Camera::Camera() {
	x = 0;
	y = 0;
	boundx0 = 0;
	boundy0 = 0;
	boundx1 = 0;
	boundy1 = 0;
	followingSprite = NULL;
}

void Camera::SetPosition(double x, double y) {
	if (HasBounds()) {
		if (x >= boundx0 && x <= boundx1 - static_cast<double>(Screen::Instance().GetWidth()))
			this->x = x;
		if (y >= boundy0 && y <= boundy1 - static_cast<double>(Screen::Instance().GetHeight()))
			this->y = y;
	}
	else {
		this->x = x;
		this->y = y;
	}
}

void Camera::SetBounds(double bx0, double by0, double bx1, double by1) {
	if (bx1 > bx0 && by1 > by0) {
		boundx0 = bx0;
		boundy0 = by0;
		boundx1 = bx1;
		boundy1 = by1;
	}
}

void Camera::FollowSprite(Sprite* sprite) {
	followingSprite = sprite;
}

void Camera::Update() {
	if (followingSprite != NULL)
		SetPosition(followingSprite->GetX() - Screen::Instance().GetWidth() / 2, followingSprite->GetY() - Screen::Instance().GetHeight() / 2);

	Renderer::Instance().SetOrigin(x, y);
}
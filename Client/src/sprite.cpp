#include "../include/sprite.h"
#include "../include/rectcollision.h"
#include "../include/image.h"
#include "../include/map.h"
#include "../include/math.h"
#include "../include/pixelcollision.h"
#include "../include/renderer.h"
#include "../include/circlecollision.h"
#include <cmath>

Sprite::Sprite(Image* image) {
	// TAREA: Implementar
	if (image != NULL) {
		this->image = image;
		radius = image->GetWidth() / 2;
	}

	x = 0;
	y = 0;
	z = 0;

	colx = 0;
	coly = 0;
	colwidth = 0;
	colheight = 0;

	angle = 0;
	scalex = 1;
	scaley = 1;

	animFPS = 0;
	firstFrame = 0;
	lastFrame = 0;
	currentFrame = 0;

	blendMode = Renderer::ALPHA;

	r = 255;
	g = 255;
	b = 255;
	a = 255;

	collision = NULL;
	colPixelData = NULL;
	colSprite = NULL;
	collided = false;

	rotating = false;
	toAngle = 0;
	rotatingSpeed = 0;
	degreesToRotate = 0;

	moving = false;
	toX = 0;
	toY = 0;
	prevX = 0;
	prevY = 0;
	speedX = 0;
	speedY = 0;
}

Sprite::~Sprite() {
    // TAREA: Implementar
	delete collision;
	delete colPixelData;
	delete colSprite;
	delete userData;
}

void Sprite::SetCollision(CollisionMode mode) {
	// TAREA: Implementar
	if (mode == COLLISION_NONE) {
		if (collision) delete collision;
		collision = NULL;
	}
	else if (mode == COLLISION_CIRCLE) {
		if (collision) delete collision;
		collision = new CircleCollision(&x, &y, &radius);
	}
	else if (mode == COLLISION_PIXEL) {
		if (collision) delete collision;
		collision = new PixelCollision(colPixelData, &colx, &coly);
	}
	else if (mode == COLLISION_RECT) {
		if (collision) delete collision;
		collision = new RectCollision(&colx, &coly, &colwidth, &colheight);
	}
}

bool Sprite::CheckCollision(Sprite* sprite) {
	// TAREA: Implementar
	bool returnValue = false;

	if (collision != NULL && sprite->GetCollision() != NULL) {
		if (collision->DoesCollide(sprite->GetCollision())) {
			colSprite = sprite;
			collided = true;
			sprite->SetColSprite(this);
			sprite->SetCollided(true);

			returnValue = true;
		}
	}
	return returnValue;
}

bool Sprite::CheckCollision(const Map* map) {
	// TAREA: Implementar
	return map->CheckCollision(collision);
}

void Sprite::RotateTo(int32 angle, double speed) {
	// TAREA: Implementar
	if (angle != this->angle) {
		toAngle = WrapValue(angle, 360);

		double firstAngle = WrapValue(toAngle - this->angle, 360);
		double secondAngle = WrapValue(this->angle - toAngle, 360);

		rotating = true;

		if (firstAngle < secondAngle) {
			degreesToRotate = firstAngle;
			rotatingSpeed = abs(speed);
		}
		else {
			degreesToRotate = secondAngle;
			rotatingSpeed = -abs(speed);
		}
	}
}

void Sprite::MoveTo(double x, double y, double speed) {
	// TAREA: Implementar
	if (this->x != x || this->y != y) {
		moving = true;
		toX = x;
		toY = y;

		double angle = Angle(this->x, this->y, toX, toY);
		speedX = DegCos(angle) * speed;
		speedY = -DegSin(angle) * speed;
	}
}

void Sprite::Update(double elapsed, const Map* map) {
	// Informacion inicial de colision
	colSprite = NULL;
	collided = false;

	// TAREA: Actualizar animacion
	// Forward
	if (lastFrame > firstFrame) {
		currentFrame += elapsed * animFPS;

		if (currentFrame > lastFrame + 1 / animFPS)
			currentFrame = firstFrame;
	}
	// Backwards
	else if (lastFrame < firstFrame) {
		currentFrame -= elapsed * animFPS;

		if (currentFrame < lastFrame - 1 / animFPS)
			currentFrame = firstFrame;
	}
	// No animation
	else if (firstFrame == lastFrame)
		currentFrame = firstFrame;

	// TAREA: Actualizar rotacion animada
	if (rotating) {
		angle += rotatingSpeed * elapsed;
		degreesToRotate -= abs(rotatingSpeed * elapsed);

		// Clamp
		if (degreesToRotate <= 0) {
			angle = toAngle;
			rotating = false;
		}
	}

	// TAREA: Actualizar movimiento animado
	if (moving) {
		// Check collision when moving automatically
		prevX = x;
		prevY = y;

		// Clamp
		if (fabs(x - (x + speedX * elapsed)) > fabs(x - toX)) {
			x = toX;

			// Map collision
			UpdateCollisionBox();
			if (CheckCollision(map)) x = prevX;
		}
		if (fabs(y - (y + speedY * elapsed)) > fabs(y - toY)) {
			y = toY;

			// Map collision
			UpdateCollisionBox();
			if (CheckCollision(map)) y = prevY;
		}

		if (x == toX && y == toY) moving = false;
		else {
			x += speedX * elapsed;

			// Map collision
			UpdateCollisionBox();
			if (CheckCollision(map)) x = prevX;

			y += speedY * elapsed;

			// Map collision
			UpdateCollisionBox();
			if (CheckCollision(map)) y = prevY;
		}

		// Stop moving
		if (x == prevX && y == prevY || 
			fabs(toX - x) > fabs(speedX * elapsed) ||
			(fabs(toY - y) > fabs(speedY * elapsed)))
			moving = false;
	}

	// Informacion final de colision
	UpdateCollisionBox();
}

void Sprite::Render() const {
    // TAREA: Implementar
	Renderer::Instance().SetColor(r, g, b, a);
	Renderer::Instance().SetBlendMode(blendMode);
	Renderer::Instance().DrawImage(image, x, y, currentFrame, image->GetWidth() * scalex, image->GetHeight() * scaley, angle);
	Renderer::Instance().SetColor(255, 255, 255, 255);
}

void Sprite::UpdateCollisionBox() {
	// TAREA: Implementar
	double cx = x - image->GetHandleX() * fabs(scalex);
	double cy = y - image->GetHandleY() * fabs(scaley);
	double cw = image->GetWidth() * fabs(scalex);
	double ch = image->GetHeight() * fabs(scaley);

	UpdateCollisionBox(cx, cy, cw, ch);
}

void Sprite::UpdateCollisionBox(double x, double y, double w, double h) {
	// TAREA: Implementar
	colx = x;
	coly = y;
	colwidth = w;
	colheight = h;
}

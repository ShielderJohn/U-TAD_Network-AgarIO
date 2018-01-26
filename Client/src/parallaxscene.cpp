#include "../include/image.h"
#include "../include/renderer.h"
#include "../include/parallaxscene.h"


ParallaxScene::ParallaxScene(Image* imageBack, Image* imageFront) {
	backLayer = imageBack;
	frontLayer = imageFront;

	backX = 0;
	backY = 0;
	frontX = 0;
	frontY = 0;

	relBackSpeedX = 0;
	relBackSpeedY = 0;
	relFrontSpeedX = 0;
	relFrontSpeedY = 0;

	autoBackSpeedX = 0;
	autoBackSpeedY = 0;
	autoFrontSpeedX = 0;
	autoFrontSpeedY = 0;
}

void ParallaxScene::Update(double elapsed, Map* map) {
	double currentCameraX = GetCamera().GetX();
	double currentCameraY = GetCamera().GetY();

	Scene::Update(elapsed, map);

	backX -= autoBackSpeedX * elapsed - relBackSpeedX * (GetCamera().GetX() - currentCameraX);
	backY -= autoBackSpeedY * elapsed - relBackSpeedY * (GetCamera().GetY() - currentCameraY);

	frontX -= autoFrontSpeedX * elapsed - relFrontSpeedX * (GetCamera().GetX() - currentCameraX);
	frontY -= autoFrontSpeedY * elapsed - relFrontSpeedY * (GetCamera().GetY() - currentCameraY);
}

void ParallaxScene::RenderBackground() const {
	Renderer::Instance().SetBlendMode(Renderer::ALPHA);

	if (backLayer != 0)
		Renderer::Instance().DrawTiledImage(backLayer, 0, 0, 2048, 1024, backX, backY);

	if (frontLayer != 0)
		Renderer::Instance().DrawTiledImage(frontLayer, 0, 0, 2048, 1024, frontX, frontY);
}
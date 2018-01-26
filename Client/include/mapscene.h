#ifndef UGINE_MAPSCENE_H
#define UGINE_MAPSCENE_H

#include "parallaxscene.h"


class MapScene : public ParallaxScene {
public:
	MapScene(Map* map, Image* imageBack = 0, Image* imageFront = 0);
	virtual const Map* GetMap() const { return map; }

	virtual void Update(double elapsed);

protected:
	virtual void RenderAfterBackground() const;

private:
	Map* map;
};

#endif
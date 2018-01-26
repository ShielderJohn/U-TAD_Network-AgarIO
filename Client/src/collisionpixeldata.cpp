#include "../include/collisionpixeldata.h"


extern "C" {
	unsigned char *stbi_load(const char *filename, int *x, int *y, int *comp, int req_comp);
	void stbi_image_free(void *buffer);
}

CollisionPixelData::CollisionPixelData(const String& filename) {
	int auxWidth, auxHeight;

	data = reinterpret_cast<bool*>(stbi_load(filename.ToCString(), &auxWidth, &auxHeight, NULL, 4));

	width = auxWidth;
	height = auxHeight;

	if (data) {
		int bufferSize = auxWidth * auxHeight * 4;

		for (int i = 0; i < bufferSize; i += 4) {
			data[i] = false;
			data[i + 1] = false;
			data[i + 2] = false;

			if	(data[i + 3] == 0)	data[i + 3] = false;
			else					data[i + 3] = true;
		}
	}
}

CollisionPixelData::~CollisionPixelData() {
	stbi_image_free(data);
}
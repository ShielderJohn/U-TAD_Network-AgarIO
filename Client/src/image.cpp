#include "../include/image.h"
#include "../include/glinclude.h"
#include "../include/math.h"
#include "../include/renderer.h"
#include <math.h>
#include <stdlib.h>

// TAREA: Declarar funciones de stb_image.c
extern "C" {
	unsigned char* stbi_load(const char* filename, int* x, int* y, int* comp, int req_comp);
	void stbi_image_free(void* buffer);
}


Image::Image(const String &filename, uint16 hframes, uint16 vframes) {
	this->filename = filename;
	this->hframes = hframes;
	this->vframes = vframes;
	width = 0;
	height = 0;
	handlex = 0;
	handley = 0;
	gltex = 0;
	lastU = 1.0;
	lastV = 1.0;

	int auxWidth = 0;
	int auxHeight = 0;

	uint16 newWidth;
	uint16 newHeight;

	// TAREA: Cargar el buffer de la imagen
	unsigned char* buffer = stbi_load(filename.ToCString(), &auxWidth, &auxHeight, NULL, 4);

	// Set original width and height
	width = static_cast<uint16>(auxWidth);
	height = static_cast<uint16>(auxHeight);

	// Get new dimensions
	newWidth = pow(2, ceil(Log2(width)));
	newHeight = pow(2, ceil(Log2(height)));

	// Check if dimensions are power of 2
	if (buffer) {
		
		if (newWidth == width && newHeight == height) {
			gltex = Renderer::Instance().GenImage(buffer, width, height);
		}

		// Create new buffer
		else {
			int newBufferSize = newHeight * newWidth * 4;
			unsigned char* newBuffer = new unsigned char[newBufferSize]();

			int oldBufferIterator;
			int newBufferIterator;

			// Fill new buffer
			for (int i = 0; i < newHeight; i++) {
				for (int j = 0; j < newWidth * 4; j += 4) {

					// Update iterator positions on both buffers
					oldBufferIterator = (i * width * 4) + j;
					newBufferIterator = (i * newWidth * 4) + j;

					// Check if we are out of bounds of old buffer
					if (j < width * 4 && i < height) {
						newBuffer[newBufferIterator] = buffer[oldBufferIterator];
						newBuffer[newBufferIterator + 1] = buffer[oldBufferIterator + 1];
						newBuffer[newBufferIterator + 2] = buffer[oldBufferIterator + 2];
						newBuffer[newBufferIterator + 3] = buffer[oldBufferIterator + 3];
					}
				}
			}

			// Generate texture
			gltex = Renderer::Instance().GenImage(newBuffer, newWidth, newHeight);
			delete[] newBuffer;

			lastU = static_cast<double>(width) / newWidth;
			lastV = static_cast<double>(height) / newHeight;

			width = newWidth;
			height = newHeight;
		}

		stbi_image_free(buffer);
	}
}

Image::~Image() {
	Renderer::Instance().DeleteImage(gltex);
}

void Image::Bind() const {
	// TAREA: Establecer la textura actual
	Renderer::Instance().BindImage(gltex);
}

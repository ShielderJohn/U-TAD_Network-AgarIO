#include "../include/string.h"
#include "../include/renderer.h"
#include "../include/glyph.h"
#include "../include/font.h"

extern "C" {
	unsigned char* stbi_load(const char* filename, int* x, int* y, int* comp, int req_comp);
	void stbi_image_free(void* buffer);
}


Font::Font(const String& filename) : Image(filename, 16, 16) {
	int width = 0;
	int height = 0;
	int bufferIterator = 0;
	int lastRedPosition = 0;

	unsigned char* buffer = stbi_load(filename.ToCString(), &width, &height, NULL, 4);

	// Iterate over the buffer
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {

			bufferIterator = (i * 4 * width) + j * 4;

			// Yellow pixel found
			if (buffer[bufferIterator] == 255 && buffer[bufferIterator + 1] == 255 && buffer[bufferIterator + 2] == 0) {
				buffer[bufferIterator + 3] = 0;
				glyphs.Add(new Glyph(j, i));
			}
			// Red pixel found
			else if (buffer[bufferIterator] == 255 && buffer[bufferIterator + 1] == 0 && buffer[bufferIterator + 2] == 0) {
				buffer[bufferIterator + 3] = 0;
				glyphs[lastRedPosition]->SetRedX(j);
				glyphs[lastRedPosition]->SetRedY(i);
				lastRedPosition++;
			}
			// Black pixel found
			else if (buffer[bufferIterator] == 0 && buffer[bufferIterator + 1] == 0 && buffer[bufferIterator + 2] == 0)
				buffer[bufferIterator + 3] = 0;
		}
	}

	gltex = Renderer::Instance().GenImage(buffer, width, height);
}

uint16 Font::GetSize() const {
	return GetHeight();
}

uint32 Font::GetTextWidth(const String& text) const {
	if (glyphs.Size() == 0)
		return GetHeight() * text.Length();
	else {
		uint32 width = 0;

		for (int i = 0; i < text.Length(); i++)
			width += glyphs[text[i]]->GetWidth();

		return width;
	}
}

uint32 Font::GetTextHeight(const String& text) const {
	if (glyphs.Size() == 0)
		return GetHeight();
	else {
		uint32 height = 0;

		for (int i = 0; i < text.Length(); i++) {
			if (glyphs[text[i]]->GetHeight() > height)
				height = glyphs[text[i]]->GetHeight();
		}

		return height;
	}
}

void Font::Render(const String& text, double x, double y) const {
	Renderer::Instance().SetBlendMode(Renderer::ALPHA);

	if (glyphs.Size() == 0) {
		for (int i = 0; i < text.Length(); i++)
			Renderer::Instance().DrawImage(this, x + (i * GetHeight()), y, text[i]);
	}
	else {
		int position = 0;

		for (int i = 0; i < text.Length(); i++) {
			Renderer::Instance().DrawImage(this, x + position, y, text[i]);
			position += glyphs[text[i]]->GetWidth();
		}
	}
}
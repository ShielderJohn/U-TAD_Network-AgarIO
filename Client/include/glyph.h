#ifndef UGINE_GLYPH_H
#define UGINE_GLYPH_H

class Glyph {
public:
	Glyph() { yellowX = 0; yellowY = 0; redX = 0; redY = 0; }
	Glyph(int yellowX, int yellowY) { this->yellowX = yellowX; this->yellowY = yellowY; }

	int GetYellowX() { return yellowX; }
	void SetYellowX(int x) { yellowX = x; }

	int GetYellowY() { return yellowY; }
	void SetYellowY(int y) { yellowY = y; }

	int GetRedX() { return redX; }
	void SetRedX(int x) { redX = x; }

	int GetRedY() { return redY; }
	void SetRedY(int y) { redY = y; }

	int GetWidth() { return redX - yellowX; }
	int GetHeight() { return redY - yellowY; }

private:
	int yellowX, yellowY;
	int redX, redY;
};

#endif
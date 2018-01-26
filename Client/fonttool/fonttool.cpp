#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_TRUETYPE_IMPLEMENTATION

#include "stb_truetype.h"
#include "stb_image_write.h"
#include "string.h"
#include "types.h"
#include <iostream>

using namespace std;

int main(int argc, char* argv[]) {
	// Check command line
    if ( argc != 3 ) {
        cout << "Usage: fonttool fontfile fontsize" << endl;
		return -1;
	}

	// Get arguments
	String fontfile = argv[1];
	int fontsize = String(argv[2]).ToInt();

	// Read file
	FILE* handle = fopen(fontfile.ToCString(), "rb");
	if ( !handle ) {
		cout << "Error: fontfile does not exist" << endl;
		return -1;
	}
	fseek(handle, 0, SEEK_END);
	long size = ftell(handle);
	fseek(handle, 0, SEEK_SET);
	uint8* buf = (uint8*)malloc(size);
	fread(buf, sizeof(uint8), size, handle);
	fclose(handle);

	// Bake font into temp alpha buffer
    stbtt_bakedchar charData[224];
    bool imgPacked = false;
    int imgsize = 32;
    uint8* alphaBuffer = NULL;
    while ( !imgPacked ) {
        alphaBuffer = (uint8*)malloc(imgsize * imgsize);
        if ( stbtt_BakeFontBitmap(buf, 0, fontsize, alphaBuffer, imgsize, imgsize, 32, 224, charData) > 0 ) {
            imgPacked = true;
        } else {
            free(alphaBuffer);
            imgsize *= 2;
        }
    }
	free(buf);

	// Copy alpha buffer into color buffer
	uint8* colorBuffer = (uint8*)malloc(imgsize * imgsize * 4);
	for ( int i = 0; i < imgsize*imgsize; i++ ) {
		colorBuffer[i*4 + 0] = alphaBuffer[i];
		colorBuffer[i*4 + 1] = alphaBuffer[i];
		colorBuffer[i*4 + 2] = alphaBuffer[i];
		colorBuffer[i*4 + 3] = alphaBuffer[i];
    }
	free(alphaBuffer);

	// Write image
	String pngfile = fontfile.StripExt() + "_" + String::FromInt(fontsize) + ".png";
	stbi_write_png(pngfile.ToCString(), imgsize, imgsize, 4, colorBuffer, 0);
	free(colorBuffer);

	// Write glyphs
	String fnt = "{\n";
	fnt += String("\t\"image\": \"") + pngfile.StripDir() + "\",\n";
	fnt += String("\t\"height\": ") + String::FromInt(fontsize) + ",\n";
	fnt += String("\t\"glyphs\":\n\t[\n");
    for ( unsigned int i = 0; i < 224; i++ ) {
        float width = charData[i].x1 - charData[i].x0;
        float height = charData[i].y1 - charData[i].y0;
        float x = charData[i].x0;
        float y = imgsize - charData[i].y0 - height;
        //float xoffset = charData[i].xoff;
        float yoffset = - (height + charData[i].yoff);
		fnt += String("\t\t{")
                + "\"x\": " + String::FromFloat(x)
                + ", \"y\": " + String::FromFloat(y)
                + ", \"width\": " + String::FromFloat(width)
                + ", \"height\": " + String::FromFloat(height)
                //+ ", \"xoffset\": " + String::FromFloat(xoffset)
                + ", \"yoffset\": " + String::FromFloat(yoffset) + "}";
        if ( i < 223 ) fnt += ",";
		fnt += "\n";
	}
	fnt += "\t]\n";
	fnt += "}\n";
	fnt.Write(fontfile.StripExt() + "_" + String::FromInt(fontsize) + ".fnt.json", false);

	return 0;
}

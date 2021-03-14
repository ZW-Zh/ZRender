
#ifndef _GLOBAL_
#define _GLOBAL_
#include "tgaimage.h"
#define PI 3.1415926
static const int width = 800;
static const int height = 800;
static const int depth = 255;


static void set_color(unsigned char* framebuffer, int x, int y, const TGAColor color)
{

	if (x >= width || x <= 0 || y >= height || y <= 0) return;
	int i;
	int index = ((height - y - 1) * width + x) * 4; // the origin for pixel is bottom-left, but the framebuffer index counts from top-left

	for (i = 0; i < 3; i++)
		//tgacolorÊÇ·´µÄ£¬bgra
		framebuffer[index + i] = color.raw[2-i];
}

template <class T>
inline int getArrayLen(T& array)

{
	return sizeof(array) / sizeof(array[0]);

}


#endif // !_GLOBAL_


#pragma once
#ifndef _GLOBAL_
#define _GLOBAL_

static const int width = 1280;
static const int height = 1000;

static void set_color(unsigned char* framebuffer, int x, int y, const unsigned char color[])
{

	if (x >= width || x <= 0 || y >= height || y <= 0) return;
	int i;
	int index = ((height - y - 1) * width + x) * 4; // the origin for pixel is bottom-left, but the framebuffer index counts from top-left

	for (i = 0; i < 3; i++)
		framebuffer[index + i] = color[i];
}

template <class T>
inline int getArrayLen(T& array)

{
	return sizeof(array) / sizeof(array[0]);

}

#endif // !_GLOBAL_


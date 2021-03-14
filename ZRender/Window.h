#pragma once
#ifndef _WINDOW_
#define _WINDOW_
#include "resource.h"
#include <Windows.h>
#include "geometry.h"

typedef struct mouse
{
	// for camera orbit
	Vec2f orbit_pos;
	Vec2f orbit_delta;
	
	// for mouse wheel
	float wheel_delta;
}mouse_t;

typedef struct window
{
	HWND h_window;
	HDC mem_dc;
	HBITMAP bm_old;
	HBITMAP bm_dib;
	unsigned char* window_fb;
	int width;
	int height;
	char keys[512];
	char buttons[2];	//left button¡ª0£¬ right button¡ª1
	int is_close;
	mouse_t mouse_info;
}window_t;

extern window_t* window;

int window_init(int width, int height);
int window_destroy();
void window_draw(unsigned char* framebuffer);
float platform_get_time(void);
void window_reset();
void msg_dispatch();
Vec2f get_mouse_pos();

#endif // !_WINDOW_


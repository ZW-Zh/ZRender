#include "Window.h"
#include <stdio.h>
#include <ctime>
#include <iostream>
#include <cassert>
#include "geometry.h"
#include "model.h"

const int width = 800;
const int height = 600;

static void set_color(unsigned char* framebuffer, int x, int y, unsigned char color[])
{
	
	if (x >= width || x <= 0 || y >= height || y <= 0) return;
	int i;
	int index = ((height - y - 1) * width + x) * 4; // the origin for pixel is bottom-left, but the framebuffer index counts from top-left

	for (i = 0; i < 3; i++)
		framebuffer[index + i] = color[i];
}

void line(int x0, int y0, int x1, int y1, unsigned char* framebuffer, unsigned char color[]) {
	/*for (float t = 0.; t < 1.; t += .1) {
		int x = x0 + (x1 - x0) * t;
		int y = y0 + (y1 - y0) * t;
		set_color(framebuffer,x, y, color);
	}
	直线陡的话，在x上采样过低；会有x1小于x的导致画不出线
	*/
	//x,y哪个差距大
	bool steep = false;
	if (std::abs(x0 - x1) < std::abs(y0 - y1)) {
		std::swap(x0, y0);
		std::swap(x1, y1);
		steep = true;
	}
	if (x0 > x1) {//使x1>=x0
		std::swap(x0, x1);
		std::swap(y0, y1);
	}
	int dx = x1 - x0;
	int dy = y1 - y0;
	//代码优化，原来除法在for循环内，耗时
	//每次累计一个误差如果误差大于一个像素 y就+1(k>0)或-1(k<0)
	//derror就是y的偏差 /1
	//float derror = std::abs(dy / float(dx));
	//float error = 0;
	//减少浮点数运算和除法
	int derror2 = std::abs(dy) * 2; //2*dy和dx比较,等价于之前的dy/dx和0.5
	int error2 = 0;
	int y = y0;
	for (int x = x0; x <= x1; x++) {
		if (steep) {//陡的话x,y会交换，显示的时候再换回来
			set_color(framebuffer, y, x, color);
		}
		else {
			set_color(framebuffer, x, y, color);
		}
		error2 += derror2;
		/*if (error > .5) {
			y += (y1 > y0 ? 1 : -1);
			error -= 1.;
		}*/
		if (error2 > dx) {
			y += (y1 > y0 ? 1 : -1);
			error2 -= dx * 2; //之前误差大于0.5减1，现在大于dx减2*dx
		}
	}

}

int main() {

	Model* model = new Model("obj/african_head.obj");

	//帧缓冲
	unsigned char* framebuffer = (unsigned char*)malloc(sizeof(unsigned char) * width * height * 4);
	assert(framebuffer != nullptr);
	memset(framebuffer, 0, sizeof(unsigned char) * width * height * 4);
	std::cout << sizeof(unsigned char) * width * height * 4;
	window_init(width, height);
	int num_frames = 0;
	float print_time = platform_get_time();
	while (!window->is_close)
	{
		float curr_time = platform_get_time();
		// calculate and display FPS
		num_frames += 1;
		if (curr_time - print_time >= 1) {
			int sum_millis = (int)((curr_time - print_time) * 1000);
			int avg_millis = sum_millis / num_frames;
			printf("fps: %3d, avg: %3d ms\n", num_frames, avg_millis);
			num_frames = 0;
			print_time = curr_time;

		}
		unsigned char color[] = { 255,0,0 };
		//画线
		//line(10, 10, 400, 300, framebuffer, color);
		for (int i = 0; i < model->nfaces(); i++) {
			std::vector<int> face = model->face(i);
			for (int j = 0; j < 3; j++) {
				Vec3f v0 = model->vert(face[j]);
				Vec3f v1 = model->vert(face[(j + 1) % 3]);
				int x0 = (v0.x + 1.) * width / 2.;
				int y0 = (v0.y + 1.) * height / 2.;
				int x1 = (v1.x + 1.) * width / 2.;
				int y1 = (v1.y + 1.) * height / 2.;
				line(x0, y0, x1, y1, framebuffer, color);
			}
		}
		// send framebuffer to window 
		window_draw(framebuffer);
		msg_dispatch();
	}
	
	free(framebuffer);
	window_destroy();

	system("pause");
	return 0;
}

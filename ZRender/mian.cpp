#include "Window.h"
#include <stdio.h>
#include <ctime>
#include <iostream>
#include <cassert>
#include "geometry.h"
#include "model.h"
#include "global.h"
#include "Triangle.h"
#include <limits>
#include "tgaimage.h"
#include "Camera.h"
#include <cmath>
#include "IShader.h"

void clear_zbuffer(int width, int height, float* zbuffer)
{
	for (int i = 0; i < width * height; i++)
		zbuffer[i] = -100000;
}

void clear_framebuffer(int width, int height, unsigned char* framebuffer)
{
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			int index = (i * width + j) * 4;

			framebuffer[index + 2] = 80;
			framebuffer[index + 1] = 56;
			framebuffer[index] = 56;
		}
	}
}

int main() {

	Model* model = new Model("obj/african_head.obj");
	TGAImage texture;
	texture.read_tga_file("obj/african_head_diffuse.tga");
	//帧缓冲
	unsigned char* framebuffer = (unsigned char*)malloc(sizeof(unsigned char) * width * height * 4);
	assert(framebuffer != nullptr);
	memset(framebuffer, 0, sizeof(unsigned char) * width * height * 4);

	//深度缓冲
	float* zbuffer = new float[width * height];
	//初始化
	for (int i = width * height; i--; zbuffer[i] = (std::numeric_limits<float>::min)());

	//Vec3f light_dir = Vec3f(1, -1, 1).normalize();
	Vec3f eye(0, 0, 3);
	Vec3f center(0, 0, 0);
	Vec3f up(0, 1, 0);
	Vec3f light_pos(10, 10, 10);

	Camera c(eye, center, up, 0);

	PhoneShader shader;

	//创建窗口
	window_init(width, height);
	int num_frames = 0;
	float print_time = platform_get_time();

	while (!window->is_close)
	{

		// clear buffer
		clear_framebuffer(width, height, framebuffer);
		clear_zbuffer(width, height, zbuffer);

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

		//模型变换
		ModelView = c.lookat();
		viewport(width, height);
		projection(60, width / height, -0.1, -10000);

		//处理鼠标事件
		handle_mouse_events(c,model);

		
		shader.light_pos = light_pos;
		shader.model = model;
		shader.image = texture;
		shader.eye = c.pos;
		
		for (int i = 0; i < model->nfaces(); i++) {
			Vec3f screen_coords[3];
			for (int j = 0; j < 3; j++) {
				screen_coords[j] = shader.vertex(i, j);
			}
			drawTriangle(framebuffer, screen_coords[0], screen_coords[1], screen_coords[2], shader, texture, zbuffer);
		}

		window_draw(framebuffer);
		msg_dispatch();
	}

	free(framebuffer);
	delete model;
	window_destroy();

	system("pause");
	return 0;
}

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

void line(int x0, int y0, int x1, int y1, unsigned char* framebuffer, TGAColor color) {
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

Matrix projection(float fovY,float aspect ,float n,float f) {
	fovY = fovY / 180.0 * PI;
	float t = std::tan(fovY / 2) * std::abs(n);
	float b = -t;
	float r = aspect * t;
	float l = -r;

	Matrix m1 = Matrix::identity();
	m1[0][0] = 2 / (r - l);
	m1[1][1] = 2 / (t - b);
	m1[2][2] = 2 / (n - f);

	Matrix m2 = Matrix::identity();
	m2[0][3] = -(r + l) / 2;
	m2[1][3] = -(t + b) / 2;
	m2[2][3] = -(n + f) / 2;

	Matrix m;
	m[0][0] = n;
	m[1][1] = n;
	m[2][2] = n + f;
	m[3][3] = 0;
	m[3][2] = 1;
	m[2][3] = - (n * f);
	
	/*Matrix m = Matrix::identity();
	fov = fov / 180.0 * PI;
	float t = fabs(near) * tan(fovy / 2);
	float r = aspect * t;

	m[0][0] = near / r;
	m[1][1] = near / t;
	m[2][2] = (near + far) / (near - far);
	m[2][3] = 2 * near * far / (far - near);
	m[3][2] = 1;
	m[3][3] = 0;
	return m;*/
	return m1 * m2 * m;
}



//视口变换[-1,1]^3->[0,width]*[0,height]
Matrix viewport(int x, int y, int w, int h) {
	Matrix m = Matrix::identity();
	m[0][3] = w / 2.f;
	m[1][3] = h / 2.f;
	m[2][3] = depth / 2.f;

	m[0][0] = w / 2.f;
	m[1][1] = h / 2.f;
	m[2][2] = depth / 2.f;
	return m;
}

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

	Vec3f light_dir = Vec3f(1, -1, 1).normalize();
	Vec3f eye(0, 0, 3);
	Vec3f center(0, 0, 0);
	Vec3f up(0, 1, 0);

	Camera c(eye, center, up, 0);
	

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
		Matrix ModelView = c.lookat();
		Matrix Projection = projection(60, width/height,-0.1,-10000);
		//Matrix Projection = Matrix::identity();
		Matrix ViewPort = viewport(0,0, width , height );
		//Projection[3][2] = -1.f / (eye - center).norm();

		/*std::cerr << ModelView << std::endl;
		std::cerr << Projection << std::endl;
		std::cerr << ViewPort << std::endl;
		Matrix z = (ViewPort * Projection * ModelView);
		std::cerr << z << std::endl;*/

		float radius = 3.0f;
		float camX = sin(platform_get_time()) * radius;
		float camZ = cos(platform_get_time()) * radius;

		c.pos[0] = camX;
		c.pos[2] = camZ;
		std::cout << c.pos << std::endl;
		//处理鼠标事件
		//handle_mouse_events(c);

		//画线
		//line(10, 10, 400, 300, framebuffer, color);
		//模型连线
		/*for (int i = 0; i < model->nfaces(); i++) {
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
		}*/
		//画三角形
		/*Vec2i pts[3] = { Vec2i(10,10), Vec2i(100, 30), Vec2i(190, 160) };
		Triangle t(pts);
		t.drawTriangle(framebuffer, color);*/
		//模型填充
		//for (int i = 0; i < model->nfaces(); i++) {
		//	std::vector<int> face = model->face(i);
		//	Vec2i screen_coords[3];
		//	for (int j = 0; j < 3; j++) {
		//		Vec3f world_coords = model->vert(face[j]);
		//		//world_coords (-1,1)
		//		screen_coords[j] = Vec2i((world_coords.x + 1.) * width / 2., (world_coords.y + 1.) * height / 2.);
		//	}
		//	unsigned char color[] = { rand() % 255, rand() % 255, rand() % 255, 255 };
		//	Triangle t(screen_coords[0], screen_coords[1], screen_coords[2]);
		//	t.drawTriangle(framebuffer, color);
		//}
		//加入光照
		Vec3f light_dir = Vec3f(1, -1, 1).normalize();
		Vec3f intensity;
		for (int i = 0; i < model->nfaces(); i++) {
			std::vector<VertexIndex> face = model->face(i);
			Vec3f screen_coords[3];
			Vec3f world_coords[3];
			for (int j = 0; j < 3; j++) {
				Vec3f v = model->vert(face[j].v);
				//世界坐标转屏幕坐标
				//[-1,1]
				//保留世界坐标z
				//screen_coords[j] = Vec3f((v.x + 1.) * width / 2. - .5, (v.y + 1.) * height / 2. - .5, v.z);
			
				//深度变换了
				screen_coords[j] = castVec3( ViewPort * Projection * ModelView * Vec4f(v,1));
				world_coords[j] = v;
				intensity[j] = model->norm_vert(face[j].norm_v) * light_dir;
			}
			//法线
			//Vec3f n = cross (world_coords[2] - world_coords[0],world_coords[1] - world_coords[0]);
			//n.normalize();
			//夹角
			//float intensity = n * light_dir;
			//if (intensity > 0) {
				//unsigned char color[] = { intensity * 255, intensity * 255, intensity * 255, 255 };
				/*Vertex v0(screen_coords[0], world_coords[0], model->texture_vert(face[0].t_v), model->norm_vert(face[0].norm_v));
				Vertex v1(screen_coords[1], world_coords[1], model->texture_vert(face[1].t_v), model->norm_vert(face[1].norm_v));
				Vertex v2(screen_coords[2], world_coords[2], model->texture_vert(face[2].t_v), model->norm_vert(face[2].norm_v));*/
				Vertex v0(screen_coords[0], world_coords[0], model->texture_vert(face[0].t_v), intensity);
				Vertex v1(screen_coords[1], world_coords[1], model->texture_vert(face[1].t_v), intensity);
				Vertex v2(screen_coords[2], world_coords[2], model->texture_vert(face[2].t_v), intensity);
				Triangle t(v0,v1,v2);
			
				t.drawTriangle(framebuffer,texture,zbuffer,color);

			//}
		}
		//此时模型的嘴被口腔覆盖了，没有做深度测试。
		// send framebuffer to window 
		
		window_draw(framebuffer);
		msg_dispatch();
	}

	free(framebuffer);
	delete model;
	window_destroy();

	system("pause");
	return 0;
}

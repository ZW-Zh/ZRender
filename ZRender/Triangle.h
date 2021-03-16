#ifndef _TRANIGLE_
#define _TRANIGLE_
#include "geometry.h"
#include "tgaimage.h"
class Triangle
{
public:
	//屏幕空间的三个顶点,包含深度
	Vec3f p0, p1, p2;
	Triangle(Vec3f t0, Vec3f t1, Vec3f t2) :p0(t0), p1(t1), p2(t2) {};
	Triangle(Vec3f v[]) :p0(v[0]), p1(v[1]), p2(v[2]) {};
	Vec3f operator[](int i) const { return i == 0 ? p0 : (i == 1 ? p1 : p2); }
	//void drawTriangle(unsigned char* framebuffer,TGAImage& image ,float* zbuffer,unsigned char color[]) const;
	//Vec3f barycentric(Vec2i P) const;

};
#endif // !_TRANIGLE_




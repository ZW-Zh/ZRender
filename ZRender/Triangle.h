#ifndef _TRANIGLE_
#define _TRANIGLE_
#include "geometry.h"
#include "tgaimage.h"
class Triangle
{
public:
	//��Ļ�ռ����������,�������
	Vertex p0, p1, p2;
	Triangle():count(0){}
	Triangle(Vertex t0, Vertex t1, Vertex t2) :p0(t0), p1(t1), p2(t2),count(3) {}
	Triangle(Vertex v[]) :p0(v[0]), p1(v[1]), p2(v[2]),count(3) {}
	Vertex operator[](int i) const { return i == 0 ? p0 : (i == 1 ? p1 : p2); }
	//void drawTriangle(unsigned char* framebuffer,TGAImage& image ,float* zbuffer,unsigned char color[]) const;
	//Vec3f barycentric(Vec2i P) const;
	void addVertex(Vertex v)
	{
		if (count == 0) {
			p0 = v;
			count++;
		}
		else if (count == 1) {
			p1 = v;
			count++;
		}
		else if (count == 2) {
			p2 = v;
			count=0;
		}
	}
private:
	int count;
};
#endif // !_TRANIGLE_




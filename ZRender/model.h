#ifndef __MODEL_H__
#define __MODEL_H__

#include <vector>
#include "geometry.h"

class Model {
private:
	std::vector<Vec3f> verts_;
	std::vector<std::vector<VertexIndex> > faces_;
	std::vector<Vec2f> texture_coords;
	std::vector<Vec3f> norms_;
public:
	Model(const char *filename);
	~Model();
	int nverts();
	int nfaces();
	Vec3f vert(int i);
	std::vector<VertexIndex> face(int idx);
	//根据索引获取纹理坐标
	Vec2f texture_vert(int i);
	Vec3f norm_vert(int i);
};

#endif //__MODEL_H__

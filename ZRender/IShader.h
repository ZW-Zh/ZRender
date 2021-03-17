#pragma once

#include "tgaimage.h"
#include "geometry.h"
#include "model.h"
#include "Triangle.h"

extern Matrix ModelView;
extern Matrix Viewport;
extern Matrix Projection;

void viewport(int w, int h);
void projection(float fovY, float aspect, float n, float f);
//void lookat(Vec3f eye, Vec3f center, Vec3f up);


struct IShader {
	Model* model;

    virtual ~IShader();
    //iface:面索引 nthvert:面中顶点索引,返回屏幕坐标
    virtual Vec3f vertex(int iface, int nthvert) = 0;
    //bar:重心坐标，返回屏幕坐标颜色
    virtual bool fragment(Vec3f bar, TGAColor& color) = 0;
};

void drawTriangle(unsigned char* framebuffer, Vec3f v0,Vec3f v1,Vec3f v2, IShader& shader, TGAImage& image, float* zbuffer);


struct GouraudShader : public IShader {
	Vec3f varying_intensity; // written by vertex shader, read by fragment shader
	Vec3f light_dir;
	virtual Vec3f vertex(int iface, int nthvert) {
		std::vector<VertexIndex> face = model->face(iface);
		Vec4f gl_Vertex = Vec4f(model->vert(face[nthvert].v), 1); // read the vertex from .obj file
		gl_Vertex = Viewport * Projection * ModelView * gl_Vertex;     // transform it to screen coordinates
		Vec3f rs_gl_Vertex = castVec3(gl_Vertex);
		varying_intensity[nthvert] = (std::max)(0.f, model->norm_vert(face[nthvert].norm_v) * light_dir); // get diffuse lighting intensity
		return rs_gl_Vertex;
	}

	virtual bool fragment(Vec3f bar, TGAColor& color) {
		float intensity = varying_intensity * bar;   // interpolate intensity for the current pixel
		color = TGAColor(255, 255, 255, 255) * intensity; // well duh
		return false;                              // no, we do not discard this pixel
	}
};

struct PhoneShader : public IShader {
	Vec3f light_dir;
	TGAImage image;
	Triangle t;

	//调用三次
	virtual Vec3f vertex(int iface, int nthvert) {
		std::vector<VertexIndex> face = model->face(iface);
		Vec3f world_coord = model->vert(face[nthvert].v);
		Vec4f gl_Vertex = Vec4f(world_coord, 1); // read the vertex from .obj file
		gl_Vertex = Viewport * Projection * ModelView * gl_Vertex;     // transform it to screen coordinates
		Vec3f rs_gl_Vertex = castVec3(gl_Vertex);
		
		//遍历
		Vertex v;
		v.norm_v = model->norm_vert(face[nthvert].norm_v);
		v.s_v = rs_gl_Vertex;
		v.t_v = model->texture_vert(face[nthvert].t_v);
		v.w_v = world_coord;
		t.addVertex(v);
		
		return rs_gl_Vertex;
	}

	virtual bool fragment(Vec3f bar, TGAColor& color) {
		float x = (t.p0.t_v.x * bar.x + t.p1.t_v.x * bar.y + t.p2.t_v.x * bar.z) * image.get_width();
		float y = image.get_height() - (t.p0.t_v.y * bar.x + t.p1.t_v.y * bar.y + t.p2.t_v.y * bar.z) * image.get_height();
		
		color = image.get(x, y);

		
		return false;                              // no, we do not discard this pixel
	}
};
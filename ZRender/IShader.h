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
	//bar:重心坐标，插值深度，返回屏幕坐标颜色
	virtual bool fragment(Vec3f bar,float z,TGAColor& color) = 0;
};

void drawTriangle(unsigned char* framebuffer, Triangle& t, IShader& shader, TGAImage& image, float* zbuffer);

struct light {
	Vec3f pos;
	Vec3f intensity;
};


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

	virtual bool fragment(Vec3f bar, float z, TGAColor& color) {
		float intensity = varying_intensity * bar;   // interpolate intensity for the current pixel
		color = TGAColor(255, 255, 255, 255) * intensity; // well duh
		return false;                              // no, we do not discard this pixel
	}
};

struct PhongShader : public IShader {
	Vec3f eye;
	Vec3f light_pos;
	TGAImage image;
	Triangle t;

	//调用三次
	virtual Vec3f vertex(int iface, int nthvert) {
		std::vector<VertexIndex> face = model->face(iface);
		Vec3f world_coord = model->vert(face[nthvert].v);
		Vec4f gl_Vertex = Vec4f(world_coord, 1); // read the vertex from .obj file
		gl_Vertex = Viewport * Projection * ModelView * gl_Vertex;     // transform it to screen coordinates
		Vec3f rs_gl_Vertex = castVec3(gl_Vertex);
		
		/*Vec4f light = Vec4f(light_pos, 1);
		light_pos = castVec3(Viewport * Projection * ModelView * light);*/
		
		//遍历
		Vertex v;
		v.norm_v = model->norm_vert(face[nthvert].norm_v);
		v.s_v = rs_gl_Vertex;
		v.t_v = model->texture_vert(face[nthvert].t_v);
		v.w_v = world_coord;
		t.addVertex(v);

		return rs_gl_Vertex;
	}

	virtual bool fragment(Vec3f bar, float z, TGAColor& color) {
		float x = (t.p0.t_v.x * bar.x + t.p1.t_v.x * bar.y + t.p2.t_v.x * bar.z) * image.get_width();
		float y = image.get_height() - (t.p0.t_v.y * bar.x + t.p1.t_v.y * bar.y + t.p2.t_v.y * bar.z) * image.get_height();
		
		Vec3f world_coord = t.p0.w_v * bar.x + t.p1.w_v * bar.y + t.p2.w_v * bar.z;
		Vec3f normal = t.p0.norm_v * bar.x + t.p1.norm_v * bar.y + t.p2.norm_v * bar.z;
		
		light l1;
		l1.pos = light_pos;
		l1.intensity.x = 200;
		l1.intensity.y = 200;
		l1.intensity.z = 200;
		TGAColor texture = image.get(x, y);
		
		Vec3f ka = Vec3f(0.025, 0.025, 0.025);
		Vec3f texture_color = Vec3f(texture.b, texture.g , texture.r);
		Vec3f ks = Vec3f(0.7937, 0.7937, 0.7937);

		Vec3f amb_light_intensity(10, 10, 10);

		
		Vec3f v = (eye - world_coord).normalize();
		Vec3f l = (light_pos - world_coord).normalize();

		int m_gls = 150;
		Vec3f specular;
		Vec3f diffuse;
		Vec3f ambient;
		
		for (size_t i = 0; i < 3; i++) {
			//ks材质镜面反射系数 * 光的强度/距离平方（光衰减） * （cos半程向量和法线夹角) ^ 光泽度
			specular[i] = texture_color[i] * ks[i] * l1.intensity[i] / ((light_pos - world_coord).norm() * (light_pos - world_coord).norm()) * std::pow((std::max)(0.f, normal * (v + l).normalize()), m_gls);
			//std::cout << (light_pos - world_coord).norm() << " " << normal * l << " "<<l1.intensity[i] / ((light_pos - world_coord).norm() * (light_pos - world_coord).norm()) * (std::max)(0.f, normal * l) <<std::endl;
			diffuse[i] = texture_color[i] * l1.intensity[i] / ((light_pos - world_coord).norm() * (light_pos - world_coord).norm()) * (std::max)(0.f,normal * l);
			ambient[i] = texture_color[i] * ka[i] * amb_light_intensity[i];
		}
		color += ambient + specular + diffuse;
		return false;                              // no, we do not discard this pixel
	}
};
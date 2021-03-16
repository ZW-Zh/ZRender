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
    //iface:面索引 nthvert:面中顶点索引
    virtual Vec3f vertex(int iface, int nthvert) = 0;
    //bar:重心坐标
    virtual bool fragment(Vec3f bar, TGAColor& color) = 0;
};

void drawTriangle(unsigned char* framebuffer, Triangle& pts, IShader& shader, TGAImage& image, float* zbuffer);



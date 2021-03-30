#include "IShader.h"
#include <cmath>
#include <limits>
#include <cstdlib>
#include "global.h"
#include "Triangle.h"

Matrix ModelView;
Matrix Viewport;
Matrix Projection;

IShader::~IShader() {}


//视口变换[-1,1]^3->[0,width]*[0,height]
void viewport(int w, int h) {
	Viewport = Matrix::identity();
	Viewport[0][3] = w / 2.f;
	Viewport[1][3] = h / 2.f;
	//Viewport[2][3] = depth / 2.f;

	Viewport[0][0] = w / 2.f;
	Viewport[1][1] = h / 2.f;
	//Viewport[2][2] = depth / 2.f;
}

void projection(float fovY, float aspect, float n, float f) {
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
	m[2][3] = -(n * f);

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
	Projection = m1 * m2 * m;
}


Vec3f barycentric(Triangle& t, Vec2i P)
{
	//叉乘
	Vec3f u = cross(Vec3f(t.p2.s_v.x - t.p0.s_v.x,t.p1.s_v.x - t.p0.s_v.x, t.p0.s_v.x - P[0]), Vec3f(t.p2.s_v.y - t.p0.s_v.y, t.p1.s_v.y - t.p0.s_v.y, t.p0.s_v.y - P[1]));
	/* `pts` and `P` has integer value as coordinates
	   so `abs(u[2])` < 1 means `u[2]` is 0, that means
	   triangle is degenerate, in this case return something with negative coordinates */
	if (std::abs(u[2]) < 1) return Vec3f(-1, 1, 1);
	return Vec3f(1.f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);

	/*float x = P.x;
	float y = P.y;
	const Triangle& pts = *this;
	float c1 = (x * (pts[1].w_v.y - pts[2].w_v.y) + (pts[2].w_v.x - pts[1].w_v.x) * y + pts[1].w_v.x * pts[2].w_v.y - pts[2].w_v.x * pts[1].w_v.y) /
		(pts[0].w_v.x * (pts[1].w_v.y - pts[2].w_v.y) + (pts[2].w_v.x - pts[1].w_v.x) * pts[0].w_v.y + pts[1].w_v.x * pts[2].w_v.y -
			pts[2].w_v.x * pts[1].w_v.y);
	float c2 = (x * (pts[2].w_v.y - pts[0].w_v.y) + (pts[0].w_v.x - pts[2].w_v.x) * y + pts[2].w_v.x * pts[0].w_v.y - pts[0].w_v.x * pts[2].w_v.y) /
		(pts[1].w_v.x * (pts[2].w_v.y - pts[0].w_v.y) + (pts[0].w_v.x - pts[2].w_v.x) * pts[1].w_v.y + pts[2].w_v.x * pts[0].w_v.y -
			pts[0].w_v.x * pts[2].w_v.y);
	float c3 = (x * (pts[0].w_v.y - pts[1].w_v.y) + (pts[1].w_v.x - pts[0].w_v.x) * y + pts[0].w_v.x * pts[1].w_v.y - pts[1].w_v.x * pts[0].w_v.y) /
		(pts[2].w_v.x * (pts[0].w_v.y - pts[1].w_v.y) + (pts[1].w_v.x - pts[0].w_v.x) * pts[2].w_v.y + pts[0].w_v.x * pts[1].w_v.y -
			pts[1].w_v.x * pts[0].w_v.y);
	return { c1, c2, c3 };*/
}

void drawTriangle(unsigned char* framebuffer, Triangle& t, IShader& shader, TGAImage& image, float* zbuffer)
{
	//屏幕空间包围盒
	float min_x = std::max(0.f, std::min(std::min(t.p0.s_v.x, t.p1.s_v.x), t.p2.s_v.x));
	float max_x = std::min((float)width, std::max(std::max(t.p0.s_v.x, t.p1.s_v.x), t.p2.s_v.x));
	float min_y = std::max(0.f, std::min(std::min(t.p0.s_v.y, t.p1.s_v.y), t.p2.s_v.y));
	float max_y = std::min((float)height, std::max(std::max(t.p0.s_v.y, t.p1.s_v.y), t.p2.s_v.y));
	for (int i = min_x; i <= max_x; i++) {
		for (int j = min_y; j <= max_y; j++) {
			Vec3f bc = barycentric(t, Vec2i(i, j));
			//坐标在三角形外
			if (bc.x < 0 || bc.y < 0 || bc.z < 0) continue;

			//加入计算深度
			//加入材质
			
			TGAColor color;
			
			//mvp变换后，视口变换没有改变z

			//透视插值矫正
			float z = 1.0 / (bc[0] / t.p0.gl_v.w + bc[1] / t.p1.gl_v.w + bc[2] / t.p2.gl_v.w);
 			//float z1 = t.p0.s_v.z * bc[0] + t.p1.s_v.z * bc[1] + t.p2.s_v.z * bc[2];

			bc[0] *= z / t.p0.gl_v.w;
			bc[1] *= z / t.p1.gl_v.w;
			bc[2] *= z / t.p2.gl_v.w;

			if (zbuffer[int(i + j * width)] < z) {
				//更新深度
				zbuffer[int(i + j * width)] = z;
				bool discard = shader.fragment(bc,z, color);
				if (!discard) {
					set_color(framebuffer, i, j, color);
				}
			}
		}
	}
}
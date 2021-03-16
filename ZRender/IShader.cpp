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
    Viewport[2][3] = depth / 2.f;

    Viewport[0][0] = w / 2.f;
    Viewport[1][1] = h / 2.f;
    Viewport[2][2] = depth / 2.f;
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


Vec3f barycentric(Triangle& pts ,Vec2i P)
{
    //叉乘
    Vec3f u = cross(Vec3f(pts[2].x - pts[0].x, pts[1].x - pts[0].x, pts[0].x - P[0]), Vec3f(pts[2].y - pts[0].y, pts[1].y - pts[0].y, pts[0].y - P[1]));
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

void drawTriangle(unsigned char* framebuffer,Triangle& pts, IShader& shader,TGAImage& image, float* zbuffer)
{
    //屏幕空间包围盒
    float min_x = std::max(0.f, std::min(std::min(pts[0].x, pts[1].x), pts[2].x));
    float max_x = std::min((float)width, std::max(std::max(pts[0].x, pts[1].x), pts[2].x));
    float min_y = std::max(0.f, std::min(std::min(pts[0].y, pts[1].y), pts[2].y));
    float max_y = std::min((float)height, std::max(std::max(pts[0].y, pts[1].y), pts[2].y));
    for (int i = min_x; i <= max_x; i++) {
        for (int j = min_y; j <= max_y; j++) {
            Vec3f bc = barycentric(pts,Vec2i(i, j));
            //坐标在三角形外
            if (bc.x < 0 || bc.y < 0 || bc.z < 0) continue;

            //加入计算深度
            //加入材质
            float z = 0;
            TGAColor color;
            //对世界坐标的深度值进行插值

            for (int k = 0; k < 3; k++)
            {
                //屏幕空间的z保留的世界坐标下的z
                z += pts[k].z * bc[k];
                //对颜色插值，因为颜色获取是int,太糊了，改为对纹理坐标插值
                //获取纹理坐标
                /*float x = image.get_width()-pts[k].t_v.x * image.get_width();
                float y = image.get_height()-pts[k].t_v.y * image.get_height();*/
                //太糊了
                /*color.r += image.get(x, y).r * bc[k];
                color.g += image.get(x, y).g * bc[k];
                color.b += image.get(x, y).b * bc[k];
                color.a += image.get(x, y).a * bc[k];*/
            }
            if (zbuffer[int(i + j * width)] < z) {
                //更新深度
                zbuffer[int(i + j * width)] = z;

                bool discard = shader.fragment(bc, color);
                if (!discard) {
                    
                    set_color(framebuffer, i, j, color);
                }
                //float x = .0, y = .0;
                ////对纹理坐标插值
                //for (int k = 0; k < 3; k++)
                //{
                //    x += pts[k].t_v.x * bc[k];
                //    y += pts[k].t_v.y * bc[k];
                //}
                //x = x * image.get_width();
                //y = image.get_height() - y * image.get_height();
                //TGAColor color = image.get(x, y);
                //set_color(framebuffer, i, j, color);

                //Gouraud shading 
                //Vec3f light_dir(0, 0, 1);
                //对顶点法线插值
                //Vec3f norm_v = pts[0].norm_v * bc[0] + pts[1].norm_v * bc[1] + pts[2].norm_v * bc[2];
                //norm_v.normalize();

                //float intensity = norm_v * light_dir;
                //float intensity = pts[0].norm_v[0] * bc[0] + pts[1].norm_v[1] * bc[1] + pts[2].norm_v[2] * bc[2];

                /*if (intensity >= 0) {
                    TGAColor color(intensity * 255, intensity * 255, intensity * 255, 255);
                    set_color(framebuffer, i, j, color);
                }*/
            }
        }
    }
}
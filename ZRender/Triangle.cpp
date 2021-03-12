#include "Triangle.h"
#include "global.h"



//old school，计算起始点，每行扫描插入，三角形水平分割为两部分
//void triangle(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage& image, TGAColor color) {
//    if (t0.y == t1.y && t0.y == t2.y) return; // I dont care about degenerate triangles 
//    // sort the vertices, t0, t1, t2 lower−to−upper (bubblesort yay!) 
//    if (t0.y > t1.y) std::swap(t0, t1);
//    if (t0.y > t2.y) std::swap(t0, t2);
//    if (t1.y > t2.y) std::swap(t1, t2);
//    int total_height = t2.y - t0.y;
//    for (int i = 0; i < total_height; i++) {
//        bool second_half = i > t1.y - t0.y || t1.y == t0.y;
//        int segment_height = second_half ? t2.y - t1.y : t1.y - t0.y;
//        float alpha = (float)i / total_height;
//        float beta = (float)(i - (second_half ? t1.y - t0.y : 0)) / segment_height; // be careful: with above conditions no division by zero here 
//        Vec2i A = t0 + (t2 - t0) * alpha;
//        Vec2i B = second_half ? t1 + (t2 - t1) * beta : t0 + (t1 - t0) * beta;
//        if (A.x > B.x) std::swap(A, B);
//        for (int j = A.x; j <= B.x; j++) {
//            image.set(j, t0.y + i, color); // attention, due to int casts t0.y+i != A.y 
//        }
//    }
//}


void Triangle::drawTriangle(unsigned char* framebuffer, TGAImage& image,float* zbuffer,unsigned char color[]) const
{
    //屏幕空间包围盒
    const Triangle& pts = *this;
    float min_x = std::min(std::min(pts[0].s_v.x, pts[1].s_v.x), pts[2].s_v.x);
    float max_x = std::max(std::max(pts[0].s_v.x, pts[1].s_v.x), pts[2].s_v.x);
    float min_y = std::min(std::min(pts[0].s_v.y, pts[1].s_v.y), pts[2].s_v.y);
    float max_y = std::max(std::max(pts[0].s_v.y, pts[1].s_v.y), pts[2].s_v.y);
    for (int i = min_x; i <= max_x; i++) {
        for (int j = min_y; j <= max_y; j++) {
            Vec3f bc = barycentric(Vec2i(i, j));
            //坐标在三角形外
            if (bc.x < 0 || bc.y < 0 || bc.z < 0) continue;

            //set_color(framebuffer, i, j, color);
            //加入计算深度
            //加入材质
            float z = 0;
            TGAColor color;
            //对世界坐标的深度值进行插值
            
            for (int k = 0; k < 3; k++)
            {
                z += pts[k].w_v.z * bc[k];
                //对颜色插值，因为颜色获取是int,太糊了，改为对纹理坐标插值
                //获取纹理坐标
                
                /*float x = image.get_width()-pts[k].t_v.x * image.get_width();
                float y = image.get_height()-pts[k].t_v.y * image.get_height();*/
               
                //太糊了
                /*color.r += image.get(x, y).r * bc[k];
                color.g += image.get(x, y).g * bc[k];
                color.b += image.get(x, y).b * bc[k];
                color.a += image.get(x, y).a * bc[k];*/

                //对坐标插值


            }
            if (zbuffer[int(i + j * width)] < z) {
                //更新深度
                zbuffer[int(i + j * width)] = z;
                float x=.0, y=.0;
                //获取纹理
                for (int k = 0; k < 3; k++)
                {
                    x +=  pts[k].t_v.x * bc[k];
                    y +=  pts[k].t_v.y * bc[k];
                }
                x = x * image.get_width();
                y = image.get_height() - y * image.get_height();
                TGAColor color = image.get(x, y);
                set_color(framebuffer, i, j, color);
            }
        }
    }
}
//二维三角形插值
Vec3f Triangle::barycentric(Vec2i P) const
{
    const Triangle& pts = *this;
    //叉乘
    Vec3f u = cross(Vec3f(pts[2].s_v.x - pts[0].s_v.x, pts[1].s_v.x - pts[0].s_v.x, pts[0].s_v.x - P[0]), Vec3f(pts[2].s_v.y - pts[0].s_v.y, pts[1].s_v.y - pts[0].s_v.y, pts[0].s_v.y - P[1]));
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

//三维三角形插值

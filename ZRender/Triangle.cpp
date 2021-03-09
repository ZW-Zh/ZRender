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


void Triangle::drawTriangle(unsigned char* framebuffer, unsigned char color[]) const
{
    const Triangle& pts = *this;
    int min_x = std::max(0, std::min(std::min(pts[0][0], pts[1][0]), pts[2][0]));
    int max_x = std::min(width - 1, std::max(std::max(pts[0][0], pts[1][0]), pts[2][0]));
    int min_y = std::max(0, std::min(std::min(pts[0][1], pts[1][1]), pts[2][1]));
    int max_y = std::min(height - 1, std::max(std::max(pts[0][1], pts[1][1]), pts[2][1]));
    for (int i = min_x; i <= max_x; i++) {
        for (int j = min_y; j <= max_y; j++) {
            Vec3f bc = barycentric(Vec2i(i, j));
            if (bc.x < 0 || bc.y < 0 || bc.z < 0) continue;
            set_color(framebuffer, i, j, color);
        }
    }
}

Vec3f Triangle::barycentric(Vec2i P) const
{
    const Triangle& pts = *this;
    //叉乘
    Vec3f u = cross(Vec3f(pts[2][0] - pts[0][0], pts[1][0] - pts[0][0], pts[0][0] - P[0]), Vec3f(pts[2][1] - pts[0][1], pts[1][1] - pts[0][1], pts[0][1] - P[1]));
    /* `pts` and `P` has integer value as coordinates
       so `abs(u[2])` < 1 means `u[2]` is 0, that means
       triangle is degenerate, in this case return something with negative coordinates */
    if (std::abs(u[2]) < 1) return Vec3f(-1, 1, 1);
    return Vec3f(1.f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);
}

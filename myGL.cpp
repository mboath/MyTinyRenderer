#include <algorithm>
#include "geometry.h"
#include "camera.h"
#include "myGL.h"

Matrix View;
Matrix Projection;
Matrix Viewport;
Matrix View_Projection_Viewport;
Matrix Shadow_View;
Matrix Shadow_View_Projection_Viewport;

void set_view(Camera* cam) {
    View = cam->get_view_matrix();
}

void set_projection(Camera* cam) {
    Projection = cam->get_projection_matrix();
}

void set_viewport(int x, int y, int w, int h, int depth) {
    Viewport = Matrix::identity(4);
    Viewport[0][3] = x + w / 2;
    Viewport[1][3] = y + h / 2;
    Viewport[2][3] = depth / 2;

    Viewport[0][0] = w / 2;
    Viewport[1][1] = h / 2;
    Viewport[2][2] = depth / 2;
}

IShader::~IShader() {}

Vec3f barycentric(const Vec4f* pts, const Vec2f& P) {
    Vec3f w = Vec3f(pts[1].x - pts[0].x, pts[2].x - pts[0].x, pts[0].x - P.x) ^ Vec3f(pts[1].y - pts[0].y, pts[2].y - pts[0].y, pts[0].y - P.y);
    if (abs(w.z) < 1e-2)
        return Vec3f(-1, 1, 1);
    float scale = 1 / w.z;
    float u = w.x * scale;
    float v = w.y * scale;
    return Vec3f(1 - u - v, u, v);
}

void triangle(const Vec4f* pts, IShader& shader, float** zbuffer, TGAImage& image) {
    //确定包围盒
    int w = image.get_width() - 1;
    int h = image.get_height() - 1;
    Vec2i bboxmin(w, h);
    Vec2i bboxmax(0, 0);
    for (int i = 0; i < 3; i++) {
        bboxmin.x = std::max(0, std::min(bboxmin.x, int(pts[i].x)));
        bboxmin.y = std::max(0, std::min(bboxmin.y, int(pts[i].y)));
        bboxmax.x = std::min(w, std::max(bboxmax.x, int(pts[i].x)));
        bboxmax.y = std::min(h, std::max(bboxmax.y, int(pts[i].y)));
    }

    Vec2f P;
    TGAColor color;
    for (int x = bboxmin.x; x <= bboxmax.x; x++) {
        for (int y = bboxmin.y; y <= bboxmax.y; y++) {
            P.x = x + 0.5;
            P.y = y + 0.5;
            //重心坐标
            Vec3f bc = barycentric(pts, P);
            if (bc.x < -1e-2 || bc.y < -1e-2 || bc.z < -1e-2)
                continue;
            //透视矫正
            Vec3f bc_corrected(bc[0] / pts[0].w, bc[1] / pts[1].w, bc[2] / pts[2].w);
            float view_z = 1.f / (bc_corrected[0] + bc_corrected[1] + bc_corrected[2]);
            bc_corrected = bc_corrected * view_z;
            if (view_z >= zbuffer[x][y]) {
                bool discard = shader.fragment(bc_corrected, color);
                if (!discard) {
                    zbuffer[x][y] = view_z;
                    image.set(x, y, color);
                }
            }
        }
    }
}
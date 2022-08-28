#include <iostream>
#include <cmath>
#include <algorithm>
#include <vector>
#include <limits>
#include "tgaimage.h"
#include "geometry.h"
#include "model.h"
#include "camera.h"
#include "myGL.h"
using namespace std;

const int width = 2000;
const int height = 2000;
const int depth = 255;

Camera* cam = new Camera(Vec3f(1, 0, 2), Vec3f(-1, 0, -2), Vec3f(0, 1, 0), -0.1, -10);
Light* light = new Light(Vec3f(3, 3, 3), Vec3f(1.f, 1.f, 1.f) * 0.3, Vec3f(1.f, 1.f, 1.f) * 1.0, Vec3f(1.f, 1.f, 1.f) * 1.0);
Material* material = new Material(TGAColor(64, 64, 64), 20.f);

Model* model = NULL;
float** shadowbuffer = NULL;
bool bilinear = true;

/*
// Blinn-Phong + Phong Shading
struct PhongShader : public IShader {
    Vec2f varying_uv[3];
    Vec3f varying_norm[3];
    Vec3f varying_fragPos[3];
    Vec4f gl_Position[3];

    virtual Vec4f vertex(int iface, int nthvert) {
        Vec4f vert = model->vert(iface, nthvert);
        varying_fragPos[nthvert] = Vec3f(vert.x, vert.y, vert.z);
        varying_uv[nthvert] = model->uv(iface, nthvert);
        varying_norm[nthvert] = model->norm(iface, nthvert);
        return (View_Projection_Viewport * vert).persp_div();
    }

    virtual bool fragment(Vec3f bar, TGAColor& color) {
        Vec2f P_uv = varying_uv[0] * bar[0] + varying_uv[1] * bar[1] + varying_uv[2] * bar[2];
        Vec3f P_norm = (varying_norm[0] * bar[0] + varying_norm[1] * bar[1] + varying_norm[2] * bar[2]).normalize();
        Vec3f P_fragPos = varying_fragPos[0] * bar[0] + varying_fragPos[1] * bar[1] + varying_fragPos[2] * bar[2];
        color = model->get_diffuse_color(P_uv, bilinear);

        //ambient
        TGAColor ambient = TGAColor(color[2] * light->ambient[2], color[1] * light->ambient[1], color[0] * light->ambient[0], color[3]);

        //diffuse
        Vec3f lightDir = (light->position - P_fragPos).normalize();
        float diff = max(lightDir * P_norm, 0.f);
        TGAColor diffuse = TGAColor(color[2] * light->diffuse[2] * diff, color[1] * light->diffuse[1] * diff, color[0] * light->diffuse[0] * diff, color[3]);

        //specular
        Vec3f viewDir = (cam->pos - P_fragPos).normalize();
        Vec3f h = (viewDir + lightDir).normalize();
        float spec = pow(max(h * P_norm, 0.f), material->shininess);
        TGAColor specular = TGAColor(material->specular[2] * light->specular[2] * spec, material->specular[1] * light->specular[1] * spec, material->specular[0] * light->specular[0] * spec, material->specular[3]);

        color = ambient + diffuse + specular;
        return false;
    }
};
*/


// Blinn-Phong + Phong Shading + Shadow
struct PhongShader : public IShader {
    Vec2f varying_uv[3];
    Vec3f varying_norm[3];
    Vec3f varying_fragPos[3];
    Vec4f gl_Position[3];

    virtual Vec4f vertex(int iface, int nthvert) {
        Vec4f vert = model->vert(iface, nthvert);
        varying_fragPos[nthvert] = Vec3f(vert.x, vert.y, vert.z);
        varying_uv[nthvert] = model->uv(iface, nthvert);
        varying_norm[nthvert] = model->norm(iface, nthvert);
        return (View_Projection_Viewport * vert).persp_div();
    }

    virtual bool fragment(Vec3f bar, TGAColor& color) {
        Vec2f P_uv = varying_uv[0] * bar[0] + varying_uv[1] * bar[1] + varying_uv[2] * bar[2];
        Vec3f P_norm = (varying_norm[0] * bar[0] + varying_norm[1] * bar[1] + varying_norm[2] * bar[2]).normalize();
        Vec3f P_fragPos = varying_fragPos[0] * bar[0] + varying_fragPos[1] * bar[1] + varying_fragPos[2] * bar[2];
        color = model->get_diffuse_color(P_uv, bilinear);

        Vec4f P_shadowViewPos = Shadow_View * Vec4f(P_fragPos.x, P_fragPos.y, P_fragPos.z, 1.f);
        Vec4f P_shadowScreenPos = (Shadow_View_Projection_Viewport * Vec4f(P_fragPos.x, P_fragPos.y, P_fragPos.z, 1.f)).persp_div();
        float shadow = 0.3 + 0.7 * (P_shadowViewPos.z > (shadowbuffer[int(P_shadowScreenPos.x)][int(P_shadowScreenPos.y)] - 0.1f));

        //ambient
        TGAColor ambient = TGAColor(color[2] * light->ambient[2], color[1] * light->ambient[1], color[0] * light->ambient[0], color[3]);

        //diffuse
        Vec3f lightDir = (light->position - P_fragPos).normalize();
        float diff = max(lightDir * P_norm, 0.f);
        TGAColor diffuse = TGAColor(color[2] * light->diffuse[2] * diff, color[1] * light->diffuse[1] * diff, color[0] * light->diffuse[0] * diff, color[3]);

        //specular
        Vec3f viewDir = (cam->pos - P_fragPos).normalize();
        Vec3f h = (viewDir + lightDir).normalize();
        float spec = pow(max(h * P_norm, 0.f), material->shininess);
        TGAColor specular = TGAColor(material->specular[2] * light->specular[2] * spec, material->specular[1] * light->specular[1] * spec, material->specular[0] * light->specular[0] * spec, material->specular[3]);

        color = ambient + (diffuse + specular) * shadow;
        return false;
    }
};


struct DepthShader : public IShader {
        Vec4f gl_Position[3];

        virtual Vec4f vertex(int iface, int nthvert) {
            return (View_Projection_Viewport * model->vert(iface, nthvert)).persp_div();
        }

        virtual bool fragment(Vec3f bar, TGAColor& color) {
            color = TGAColor(255, 255, 255);
            return false;
        }
};

int main(int argc, char** argv)
{
    //创建画布
    TGAImage frame(width, height, TGAImage::RGB);
    TGAImage depth(width, height, TGAImage::RGB);

    //深度缓冲
    float** zbuffer = new float* [width];
    for (int i = 0; i < width; i++) {
        zbuffer[i] = new float[height];
        memset(zbuffer[i], -numeric_limits<float>::max(), sizeof(zbuffer[i]));
    }

    //阴影缓冲
    shadowbuffer = new float* [width];
    for (int i = 0; i < width; i++) {
        shadowbuffer[i] = new float[height];
        memset(zbuffer[i], -numeric_limits<float>::max(), sizeof(shadowbuffer[i]));
    }

    //first pass: rendering the depth buffer
    Camera* shadow_cam = new Camera(light->position, light->position * (-1), Vec3f(0, 1, 0), -0.1, -100);
    set_view(shadow_cam);
    set_projection(shadow_cam);
    set_viewport(0, 0, width, height);
    View_Projection_Viewport = Viewport * Projection * View;
    Shadow_View = View;
    Shadow_View_Projection_Viewport = View_Projection_Viewport;

    DepthShader depth_shader;
    for (int m = 1; m < argc; m ++) {
        model = new Model(argv[m]);
        for (int i = 0; i < model->nfaces(); i++) {
            for (int j = 0; j < 3; j++)
                depth_shader.gl_Position[j] = depth_shader.vertex(i, j);

            triangle(depth_shader.gl_Position, depth_shader, shadowbuffer, depth);
        }
    }
    
    //second pass: rendering the frame buffer
    set_view(cam);
    set_projection(cam);
    set_viewport(0, 0, width, height);
    View_Projection_Viewport = Viewport * Projection * View;

    PhongShader phong_shader;
    for (int m = 1; m < argc; m++) {      
        model = new Model(argv[m]);
        for (int i = 0; i < model->nfaces(); i++) {
            for (int j = 0; j < 3; j++)
                phong_shader.gl_Position[j] = phong_shader.vertex(i, j);

            triangle(phong_shader.gl_Position, phong_shader, zbuffer, frame);
        }
    }

    frame.flip_vertically();
    frame.write_tga_file("output.tga");
    depth.flip_vertically();
    depth.write_tga_file("depth.tga");
    delete model;
    delete cam;
    delete shadow_cam;
    delete light;
    delete material;
    for (int i = 0; i < width; i++)
        delete[] zbuffer[i];
    delete[] zbuffer;
    return 0;
}


#ifndef __MY_GL_H__
#define __MY_GL_H__

#include "tgaimage.h"
#include "geometry.h"
#include "camera.h"

extern Matrix View;
extern Matrix Projection;
extern Matrix Viewport;
extern Matrix View_Projection_Viewport;
extern Matrix Shadow_View;
extern Matrix Shadow_View_Projection_Viewport;

void set_view(Camera* cam);
void set_projection(Camera* cam);
void set_viewport(int x, int y, int w, int h, int depth = 255);

struct IShader {
    virtual ~IShader();
    virtual Vec4f vertex(int iface, int nthvert) = 0;
    virtual bool fragment(Vec3f bar, TGAColor& color) = 0;
};

void triangle(const Vec4f* pts, IShader& shader, float** zbuffer, TGAImage& image);

struct Light {
    Vec3f position;
    Vec3f ambient;
    Vec3f diffuse;
    Vec3f specular;
    Light(const Vec3f& _position, const Vec3f& _ambient, const Vec3f& _diffuse, const Vec3f& _specular) :
        position(_position), ambient(_ambient), diffuse(_diffuse), specular(_specular) {}
};

struct Material {
    TGAColor ambient;
    TGAColor diffuse;
    TGAColor specular;
    float shininess;
    Material(const TGAColor& _ambient, const TGAColor& _diffuse, const TGAColor& _specular, float _shininess) :
        ambient(_ambient), diffuse(_diffuse), specular(_specular), shininess(_shininess) {}
    Material(const TGAColor& _specular, float _shininess) : specular(_specular), shininess(_shininess) {}
};

#endif //__MY_GL_H__

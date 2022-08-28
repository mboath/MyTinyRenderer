#ifndef __MODEL_H__
#define __MODEL_H__

#include <vector>
#include "geometry.h"
#include "texture.h"

class Model {
private:
	std::vector<Vec4f> verts_;
	std::vector<Vec2f> uvs_;
	std::vector<Vec3f> norms_;
	std::vector<std::vector<Vec3i>> faces_;
	Texture* texture_ = nullptr;
public:
	Model(const char* filename);
	~Model();
	int nverts();
	int nfaces();
	Vec4f vert(int i);
	Vec2f uv(int i);
	Vec3f norm(int i);
	std::vector<Vec3i> face(int i);
	Vec4f vert(int iface, int nthvert);
	Vec2f uv(int iface, int nthvert);
	Vec3f norm(int iface, int nthvert);
	TGAColor get_diffuse_color(const Vec2f& uv, bool bilinear);
};

#endif //__MODEL_H__

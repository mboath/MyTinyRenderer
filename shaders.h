#ifndef __SHADERS_H__
#define __SHADERS_H__

class PhongShader : public IShader {
	Vec2f varying_uv[3];
	Vec3f varying_norm[3];
	
	virtual Vec4f vertex(int iface, int nthvert) {
		Vec4f tmp = ViewProjection * model->vert(face[j][0]);
		verts[j] = Viewport * tmp.persp_div();
		verts[j][3] = tmp[3];
		uvs[j] = model->uv(face[j][1]);
		norms[j] = model->norm(face[j][2]);
	}

	virtual bool fragment(Vec3f bar, TGAColor& color) {

	}
};

#endif //__SHADERS_H__

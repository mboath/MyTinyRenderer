#ifndef __CAMERA_H__
#define __CAMERA_H__

#include <cmath>
#include "geometry.h"

#define PI acos(-1)

struct Camera {
	Vec3f pos;
	Vec3f front;
	Vec3f up;
	float n, f, fovY, aspect;

	Camera(const Vec3f& pos_, const Vec3f& front_, const Vec3f& up_, float n_, float f_, float fovY_ = 90.f, float aspect_ = 1.f) :
		pos(pos_), front(front_), up(up_), n(n_), f(f_), fovY(fovY_), aspect(aspect_) {
		front.normalize();
		up.normalize();
	}

	Matrix get_view_matrix() {
		Matrix T = Matrix::identity(4);
		for (int i = 0; i < 3; i++)
			T[i][3] = -pos[i];

		Matrix R = Matrix::identity(4);
		Vec3f right = front ^ up;
		up = right ^ front;
		for (int j = 0; j < 3; j++) {
			R[0][j] = right[j];
			R[1][j] = up[j];
			R[2][j] = -front[j];
		}

		return R * T;
	}

	Matrix get_projection_matrix() {
		Matrix m;
		float t = abs(n) * tan(fovY/2 * PI/180);
		float r = t * aspect;
		m[0][0] = n / r;
		m[1][1] = n / t;
		m[2][2] = (n + f) / (n - f);
		m[2][3] = -2 * n * f / (n - f);
		m[3][2] = 1;
		return m;
	}
};

#endif //__CAMERA_H__
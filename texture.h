#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include "tgaimage.h"
#include "geometry.h"

TGAColor lerp(const TGAColor& c1, const TGAColor& c2, float s);

class Texture {
private:
	TGAImage image;
    int width;
    int height;
public:
	Texture(const char* filename) {
        std::cerr << "texture file " << filename << " loading " << (image.read_tga_file(filename) ? "succeeded" : "failed") << std::endl;
		image.flip_vertically();
        width = image.get_width();
        height = image.get_height();
	}

    TGAColor getColor(const Vec2f& uv, bool bilinear) {
        Vec2f texCoords(uv.x * width, uv.y * height);
        if (!bilinear) {
            return image.get((int)texCoords.x, (int)texCoords.y);
        }
        else {
            int mid_x = (int)(texCoords.x + 0.5);
            int mid_y = (int)(texCoords.y + 0.5);
            Vec2i near[4] = { Vec2i(mid_x - 1, mid_y - 1),
                                       Vec2i(mid_x,      mid_y - 1),
                                       Vec2i(mid_x - 1, mid_y),
                                       Vec2i(mid_x,      mid_y) };
            TGAColor c[4];
            for (int i = 0; i < 4; i++)
                c[i] = image.get(near[i].x, near[i].y);
            float s = texCoords.x - mid_x + 0.5;
            float t = texCoords.y - mid_y + 0.5;
            return lerp(lerp(c[0], c[1], s), lerp(c[2], c[3], s), t);
        }
    }
};

#endif //__TEXTURE_H__
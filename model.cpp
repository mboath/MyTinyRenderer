#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "model.h"

Model::Model(const char* filename) : verts_(), uvs_(), norms_(), faces_() {
    std::ifstream in;
    in.open(filename, std::ifstream::in);
    if (in.fail()) return;
    std::string line;
    while (!in.eof()) {
        std::getline(in, line);
        std::istringstream iss(line.c_str());
        char trash;
        if (!line.compare(0, 2, "v ")) {
            iss >> trash;
            Vec4f v(0.f, 0.f, 0.f, 1.f);
            for (int i = 0; i < 3; i++) iss >> v[i];
            verts_.push_back(v);
        }
        else if (!line.compare(0, 3, "vt ")) {
            iss >> trash >> trash;
            Vec2f vt;
            for (int i = 0; i < 2; i++) iss >> vt[i];
            uvs_.push_back(vt);
        }
        else if (!line.compare(0, 3, "vn ")) {
            iss >> trash >> trash;
            Vec3f vn;
            for (int i = 0; i < 3; i++) iss >> vn[i];
            norms_.push_back(vn);
        }
        else if (!line.compare(0, 2, "f ")) {
            std::vector<Vec3i> f;
            Vec3i v;
            iss >> trash;
            while (iss >> v[0] >> trash >> v[1] >> trash >> v[2]) {
                for (int i = 0; i < 3; i++) v[i]--; // in wavefront obj all indices start at 1, not zero
                f.push_back(v);
            }
            faces_.push_back(f);
        }
    }
    std::cerr << "# v# " << verts_.size() << " vt# " << uvs_.size() << " vn# " << norms_.size() << " f# "  << faces_.size() << std::endl;
    
    std::string filename_str(filename);
    size_t dot = filename_str.find_last_of(".");
    if (dot != std::string::npos) {
        filename_str = filename_str.substr(0, dot);
        texture_ = new Texture((filename_str + std::string("_diffuse.tga")).c_str());
    }
}

Model::~Model() {
    delete texture_;
}

int Model::nverts() {
    return (int)verts_.size();
}

int Model::nfaces() {
    return (int)faces_.size();
}

Vec4f Model::vert(int i) {
    return verts_[i];
}

Vec2f Model::uv(int i) {
    return uvs_[i];
}

Vec3f Model::norm(int i) {
    return norms_[i];
}

std::vector<Vec3i> Model::face(int i) {
    return faces_[i];
}

Vec4f Model::vert(int iface, int nthvert) {
    return verts_[faces_[iface][nthvert][0]];
}

Vec2f Model::uv(int iface, int nthvert) {
    return uvs_[faces_[iface][nthvert][1]];
}

Vec3f Model::norm(int iface, int nthvert) {
    return norms_[faces_[iface][nthvert][2]];
}

TGAColor Model::get_diffuse_color(const Vec2f& uv, bool bilinear) {
    return texture_->getColor(uv, bilinear);
}

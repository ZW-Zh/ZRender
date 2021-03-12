#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "model.h"

Model::Model(const char *filename) : verts_(), faces_() {
    std::ifstream in;
    in.open (filename, std::ifstream::in);
    if (in.fail()) return;
    std::string line;
    while (!in.eof()) {
        std::getline(in, line);
        std::istringstream iss(line.c_str());
        char trash;
        if (!line.compare(0, 2, "v ")) {
            iss >> trash;
            Vec3f v;
            for (int i=0;i<3;i++) iss >> v[i];
            verts_.push_back(v);
        }
        else if (!line.compare(0, 2, "f ")) {
            std::vector<VertexIndex> f;
            int itrash, idx, vn;
            iss >> trash;
            while (iss >> idx >> trash >> itrash >> trash >> vn) {
                idx--; // in wavefront obj all indices start at 1, not zero
                itrash--;
                VertexIndex v(idx, itrash);
                f.push_back(v);
            }
            faces_.push_back(f);
        }
        else if (!line.compare(0, 3, "vt ")) {
            iss >> trash >>trash;
            Vec3f v;
            for (int i = 0; i < 3; i++) iss >> v[i];
            texture_coords.push_back(v);
        }
    }
    std::cerr << "# v# " << verts_.size() << " f# "  << faces_.size() << std::endl;
}

Model::~Model() {
}

int Model::nverts() {
    return (int)verts_.size();
}

int Model::nfaces() {
    return (int)faces_.size();
}

std::vector<VertexIndex> Model::face(int idx) {
    return faces_[idx];
}

Vec3f Model::texture_vert(int i)
{
    return texture_coords[i];
}

Vec3f Model::vert(int i) {
    return verts_[i];
}


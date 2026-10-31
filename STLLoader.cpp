#include "STLLoader.h"
#include <fstream>
#include <iostream>

bool STLLoader::loadBinarySTL(const std::string& path,
    std::vector<STLVertex>& vertices,
    std::vector<unsigned int>& indices) {
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open STL file: " << path << std::endl;
        return false;
    }

    char header[80];
    file.read(header, 80);

    unsigned int triangleCount;
    file.read((char*)&triangleCount, 4);

    vertices.clear();
    indices.clear();

    for (unsigned int i = 0; i < triangleCount; ++i) {
        float normal[3];
        float v1[3], v2[3], v3[3];
        file.read((char*)normal, 12);
        file.read((char*)v1, 12);
        file.read((char*)v2, 12);
        file.read((char*)v3, 12);

        STLVertex vert1, vert2, vert3;
        vert1.position = glm::vec3(v1[0], v1[1], v1[2]);
        vert2.position = glm::vec3(v2[0], v2[1], v2[2]);
        vert3.position = glm::vec3(v3[0], v3[1], v3[2]);
        vert1.normal = glm::vec3(normal[0], normal[1], normal[2]);
        vert2.normal = vert1.normal;
        vert3.normal = vert1.normal;

        unsigned int base = vertices.size();
        vertices.push_back(vert1);
        vertices.push_back(vert2);
        vertices.push_back(vert3);
        indices.push_back(base);
        indices.push_back(base + 1);
        indices.push_back(base + 2);

        unsigned short attr;
        file.read((char*)&attr, 2);
    }
    file.close();
    return true;
}
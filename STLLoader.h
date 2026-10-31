#pragma once
#include <vector>
#include <string>
#include <glm/glm.hpp>

struct STLVertex {
    glm::vec3 position;
    glm::vec3 normal;
};

class STLLoader {
public:
    static bool loadBinarySTL(const std::string& path,
        std::vector<STLVertex>& vertices,
        std::vector<unsigned int>& indices);
};

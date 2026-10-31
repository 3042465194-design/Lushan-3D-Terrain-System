#include "ModelNode.h"
#include "Renderer.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <cfloat>
#include "stb_image.h"

ModelNode::ModelNode(const std::string& objPath, const std::string& texPath)
    : m_VAO(0), m_VBO(0), m_EBO(0), m_textureID(0), m_indexCount(0),
    m_useElevationColor(true), m_minY(0), m_maxY(1), m_valid(false),
    m_minX(FLT_MAX), m_maxX(-FLT_MAX), m_minZ(FLT_MAX), m_maxZ(-FLT_MAX)
{
    if (!loadOBJ(objPath)) {
        std::cerr << "Failed to load OBJ: " << objPath << std::endl;
        return;
    }

    computeHeightRange();
    computeTexCoords();
    setupBuffers();
    if (!loadTexture(texPath)) {
        std::cerr << "Warning: Failed to load texture: " << texPath << std::endl;
    }

    m_valid = true;
    std::cout << "Model loaded successfully. Vertices: " << m_vertices.size()
        << ", Triangles: " << m_indexCount / 3 << std::endl;
    std::cout << "World X range: " << m_minX << " ~ " << m_maxX << std::endl;
    std::cout << "World Z range: " << m_minZ << " ~ " << m_maxZ << std::endl;
    std::cout << "Height range (Y): " << m_minY << " ~ " << m_maxY << std::endl;
}

ModelNode::~ModelNode() {
    if (m_VAO) glDeleteVertexArrays(1, &m_VAO);
    if (m_VBO) glDeleteBuffers(1, &m_VBO);
    if (m_EBO) glDeleteBuffers(1, &m_EBO);
    if (m_textureID) glDeleteTextures(1, &m_textureID);
}

bool ModelNode::loadOBJ(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) return false;

    std::vector<glm::vec3> tempPos; 
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string type;
        iss >> type;
        if (type == "v") {
            glm::vec3 pos;
            iss >> pos.x >> pos.y >> pos.z;
            //交换 Y 和 Z，将 (x, y, z) 转换为世界坐标 (x, z, y)
            glm::vec3 worldPos(pos.x, pos.z, pos.y);
            tempPos.push_back(worldPos);
            if (worldPos.x < m_minX) m_minX = worldPos.x;
            if (worldPos.x > m_maxX) m_maxX = worldPos.x;
            if (worldPos.z < m_minZ) m_minZ = worldPos.z;
            if (worldPos.z > m_maxZ) m_maxZ = worldPos.z;
        }
        else if (type == "f") {
            std::string v1, v2, v3;
            iss >> v1 >> v2 >> v3;
            auto parseIndex = [](const std::string& s) -> int {
                size_t pos = s.find('/');
                std::string idxStr = (pos == std::string::npos) ? s : s.substr(0, pos);
                return std::stoi(idxStr) - 1;
                };
            int i1 = parseIndex(v1);
            int i2 = parseIndex(v2);
            int i3 = parseIndex(v3);
            Vertex vert1, vert2, vert3;
            vert1.position = tempPos[i1];
            vert2.position = tempPos[i2];
            vert3.position = tempPos[i3];
            unsigned int base = (unsigned int)m_vertices.size();
            m_vertices.push_back(vert1);
            m_vertices.push_back(vert2);
            m_vertices.push_back(vert3);
            m_indices.push_back(base);
            m_indices.push_back(base + 1);
            m_indices.push_back(base + 2);
        }
    }
    m_indexCount = (int)m_indices.size();
    return !m_vertices.empty();
}

void ModelNode::computeHeightRange() {
    if (m_vertices.empty()) return;
    m_minY = m_maxY = m_vertices[0].position.y; // 使用 Y 作为高程
    for (const auto& v : m_vertices) {
        if (v.position.y < m_minY) m_minY = v.position.y;
        if (v.position.y > m_maxY) m_maxY = v.position.y;
    }
}

void ModelNode::computeTexCoords() {
    if (m_vertices.empty()) return;
    // 计算 X 和 Z 的范围（平面范围）
    float minX = FLT_MAX, maxX = -FLT_MAX;
    float minZ = FLT_MAX, maxZ = -FLT_MAX;
    for (const auto& vert : m_vertices) {
        if (vert.position.x < minX) minX = vert.position.x;
        if (vert.position.x > maxX) maxX = vert.position.x;
        if (vert.position.z < minZ) minZ = vert.position.z;
        if (vert.position.z > maxZ) maxZ = vert.position.z;
    }
    float rangeX = maxX - minX;
    float rangeZ = maxZ - minZ;
    if (rangeX == 0.0f) rangeX = 1.0f;
    if (rangeZ == 0.0f) rangeZ = 1.0f;
    for (auto& vert : m_vertices) {
        float u = (vert.position.x - minX) / rangeX;
        float v = (vert.position.z - minZ) / rangeZ;
        vert.texCoord = glm::vec2(u, v);
    }
}

void ModelNode::setupBuffers() {
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);

    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex), m_vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), m_indices.data(), GL_STATIC_DRAW);

    // 位置属性 (x, y, z)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    glEnableVertexAttribArray(0);
    // 纹理坐标属性 (u, v)
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

bool ModelNode::loadTexture(const std::string& path) {
    int width, height, channels;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
    if (!data) return false;
    glGenTextures(1, &m_textureID);
    glBindTexture(GL_TEXTURE_2D, m_textureID);
    GLenum format = (channels == 3) ? GL_RGB : (channels == 4) ? GL_RGBA : GL_RED;
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    stbi_image_free(data);
    return true;
}

void ModelNode::update(float deltaTime) {
    // 预留
}

void ModelNode::draw(Renderer* renderer) {
    if (!m_valid) return;
    glBindVertexArray(m_VAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_textureID);
    glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
#include "TerrainNode.h"
#include "Renderer.h"
#include <vector>

TerrainNode::TerrainNode(int width, int depth, float spacing, const std::vector<float>& heightData)
    : m_width(width), m_depth(depth), m_spacing(spacing), m_heightData(heightData), m_VAO(0), m_VBO(0), m_EBO(0)
{
    generateMesh();
}

TerrainNode::~TerrainNode() {
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
    glDeleteBuffers(1, &m_EBO);
}

void TerrainNode::setupBuffers() {
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> texCoords;
    std::vector<unsigned int> indices;

    for (int z = 0; z < m_depth; ++z) {
        for (int x = 0; x < m_width; ++x) {
            float y = m_heightData[z * m_width + x];
            vertices.push_back(glm::vec3(x * m_spacing, y, z * m_spacing));
            texCoords.push_back(glm::vec2((float)x / (m_width - 1), (float)z / (m_depth - 1)));
        }
    }
    for (int z = 0; z < m_depth - 1; ++z) {
        for (int x = 0; x < m_width - 1; ++x) {
            int i = z * m_width + x;
            indices.push_back(i);
            indices.push_back(i + m_width);
            indices.push_back(i + 1);
            indices.push_back(i + 1);
            indices.push_back(i + m_width);
            indices.push_back(i + m_width + 1);
        }
    }
    m_indexCount = indices.size();

    std::vector<float> interleaved;
    for (size_t i = 0; i < vertices.size(); ++i) {
        interleaved.push_back(vertices[i].x);
        interleaved.push_back(vertices[i].y);
        interleaved.push_back(vertices[i].z);
        interleaved.push_back(texCoords[i].x);
        interleaved.push_back(texCoords[i].y);
    }

    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);

    glBindVertexArray(m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, interleaved.size() * sizeof(float), interleaved.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // 位置属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // 纹理坐标属性
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void TerrainNode::draw(Renderer* renderer) {
    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
// 地形节点的更新逻辑（可暂留空）
void TerrainNode::update(float deltaTime) {
    // 后续如果需要动态 LOD 或动画，可在此实现
}

// 生成网格数据（如果已有 setupBuffers，可以调用它）
void TerrainNode::generateMesh() {
    // 因为 setupBuffers 已经完成了网格生成和缓冲上传，
    // 这里直接调用即可（注意避免重复调用）
    setupBuffers();
}
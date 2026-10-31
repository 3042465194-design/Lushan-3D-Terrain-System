#include "PlaneNode.h"
#include "Renderer.h"
#include <glad/glad.h>

PlaneNode::PlaneNode(const std::string& stlPath, float scale)
    : m_VAO(0), m_VBO(0), m_EBO(0), m_position(0.0f), m_rotationY(0.0f) {
    std::vector<STLVertex> rawVertices;
    STLLoader::loadBinarySTL(stlPath, rawVertices, m_indices);

    // 缩放顶点
    m_vertices.resize(rawVertices.size());
    for (size_t i = 0; i < rawVertices.size(); ++i) {
        m_vertices[i].position = rawVertices[i].position * scale;
        m_vertices[i].normal = rawVertices[i].normal; 
    }
    setupBuffers();
}

PlaneNode::~PlaneNode() {
    if (m_VAO) glDeleteVertexArrays(1, &m_VAO);
    if (m_VBO) glDeleteBuffers(1, &m_VBO);
    if (m_EBO) glDeleteBuffers(1, &m_EBO);
}

void PlaneNode::setupBuffers() {
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);

    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(STLVertex), m_vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), m_indices.data(), GL_STATIC_DRAW);

    // 位置属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(STLVertex), (void*)offsetof(STLVertex, position));
    glEnableVertexAttribArray(0);
    // 法线属性
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(STLVertex), (void*)offsetof(STLVertex, normal));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void PlaneNode::update(float deltaTime) {
    // 可在此添加动画逻辑
}

void PlaneNode::draw(Renderer* renderer) {
    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
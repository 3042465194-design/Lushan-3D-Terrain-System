#include "SkyDomeNode.h"
#include "Renderer.h"
#include <glad/glad.h>
#include <cmath>

#ifndef PI
#define PI 3.14159265358979323846f
#endif

static float clamp(float x, float a, float b) {
    return x < a ? a : (x > b ? b : x);
}

SkyDomeNode::SkyDomeNode(float radius, int rings, int segments)
    : m_VAO(0), m_VBO(0), m_EBO(0), m_indexCount(0) {
    buildDome(radius, rings, segments);
}

SkyDomeNode::~SkyDomeNode() {
    if (m_VAO) glDeleteVertexArrays(1, &m_VAO);
    if (m_VBO) glDeleteBuffers(1, &m_VBO);
    if (m_EBO) glDeleteBuffers(1, &m_EBO);
}

void SkyDomeNode::buildDome(float radius, int rings, int segments) {
    m_vertices.clear();
    m_indices.clear();

    // 上半球
    for (int i = 0; i <= rings; ++i) {
        float t0 = (float)i / rings;          // 0 = 地平线, 1 = 天顶
        float theta = t0 * PI * 0.5f;
        float sinTheta = sinf(theta);
        float cosTheta = cosf(theta);
        float r = radius * cosTheta;
        float y = radius * sinTheta;

        // 计算颜色
        float t = sinTheta;   // 0 地平线 -> 1 天顶
        t = clamp(t, 0.0f, 1.0f);
        float rCol, gCol, bCol;
        if (t < 0.55f) {
            float k = t / 0.55f;
            rCol = 0.78f + (0.45f - 0.78f) * k;
            gCol = 0.88f + (0.66f - 0.88f) * k;
            bCol = 0.98f + (0.92f - 0.98f) * k;
        }
        else {
            float k = (t - 0.55f) / 0.45f;
            rCol = 0.45f + (0.16f - 0.45f) * k;
            gCol = 0.66f + (0.38f - 0.66f) * k;
            bCol = 0.92f + (0.74f - 0.92f) * k;
        }

        for (int j = 0; j <= segments; ++j) {
            float phi = (float)j / segments * PI * 2.0f;
            float cx = r * cosf(phi);
            float cz = r * sinf(phi);
            Vertex v;
            v.position = glm::vec3(cx, y, cz);
            v.color = glm::vec3(rCol, gCol, bCol);
            m_vertices.push_back(v);
        }
    }

    // 索引（三角形条带）
    for (int i = 0; i < rings; ++i) {
        for (int j = 0; j < segments; ++j) {
            int first = i * (segments + 1) + j;
            int second = first + segments + 1;
            m_indices.push_back(first);
            m_indices.push_back(second);
            m_indices.push_back(first + 1);

            m_indices.push_back(second);
            m_indices.push_back(second + 1);
            m_indices.push_back(first + 1);
        }
    }
    m_indexCount = (int)m_indices.size();

    // 生成地平线以下的裙边
    int skirtRings = 1;
    float skirtDepth = radius * 0.10f;
    int baseIndex = (rings + 1) * (segments + 1);
    for (int i = 0; i <= skirtRings; ++i) {
        float yOffset = (i == 0) ? 0.0f : -skirtDepth;
        float r = radius;
        for (int j = 0; j <= segments; ++j) {
            float phi = (float)j / segments * PI * 2.0f;
            float cx = r * cosf(phi);
            float cz = r * sinf(phi);
            Vertex v;
            v.position = glm::vec3(cx, yOffset, cz);
            v.color = (i == 0) ? glm::vec3(0.78f, 0.88f, 0.98f) : glm::vec3(0.58f, 0.66f, 0.72f);
            m_vertices.push_back(v);
        }
    }
    for (int j = 0; j < segments; ++j) {
        int p0 = baseIndex + j;
        int p1 = baseIndex + j + 1;
        int p2 = baseIndex + (segments + 1) + j;
        int p3 = baseIndex + (segments + 1) + j + 1;
        m_indices.push_back(p0);
        m_indices.push_back(p1);
        m_indices.push_back(p2);
        m_indices.push_back(p1);
        m_indices.push_back(p3);
        m_indices.push_back(p2);
    }
    m_indexCount = (int)m_indices.size();

    // 创建缓冲区
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);

    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex), m_vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), m_indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void SkyDomeNode::update(float deltaTime) {}

void SkyDomeNode::draw(Renderer* renderer) {
    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
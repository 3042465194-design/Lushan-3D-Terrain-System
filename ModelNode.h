#pragma once
#include "SceneNode.h"
#include <string>
#include <vector>
#include <glm/glm.hpp>

class ModelNode : public SceneNode {
public:
    ModelNode(const std::string& objPath, const std::string& texPath);
    virtual ~ModelNode();

    void update(float deltaTime) override;
    void draw(Renderer* renderer) override;

    bool isValid() const { return m_valid; }
    float getMinHeight() const { return m_minY; }   // 高程最小值（Y轴）
    float getMaxHeight() const { return m_maxY; }   // 高程最大值（Y轴）
    void setElevationColorEnabled(bool enable) { m_useElevationColor = enable; }

    // 获取地形平面范围
    float getMinX() const { return m_minX; }
    float getMaxX() const { return m_maxX; }
    float getMinZ() const { return m_minZ; }
    float getMaxZ() const { return m_maxZ; }

private:
    bool loadOBJ(const std::string& path);
    void computeTexCoords();      // 基于 X,Z 生成纹理坐标
    void computeHeightRange();    // 计算 Y 轴范围
    void setupBuffers();
    bool loadTexture(const std::string& path);

    struct Vertex {
        glm::vec3 position;   // 世界坐标 (x, y, z)  y 为高程
        glm::vec2 texCoord;
    };
    std::vector<Vertex> m_vertices;
    std::vector<unsigned int> m_indices;

    GLuint m_VAO, m_VBO, m_EBO;
    GLuint m_textureID;
    int m_indexCount;

    bool m_useElevationColor;
    float m_minY, m_maxY;      // 高程范围（Y轴）
    float m_minX, m_maxX, m_minZ, m_maxZ;  // 平面范围（X和Z轴）
    bool m_valid;
};
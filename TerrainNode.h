#pragma once
#include "SceneNode.h"

class TerrainNode : public SceneNode {
public:
    TerrainNode(int width, int depth, float spacing, const std::vector<float>& heightData);
    virtual ~TerrainNode();

    void update(float deltaTime) override;
    void draw(Renderer* renderer) override;

private:
    void generateMesh();
    void setupBuffers();

    int m_width, m_depth;
    float m_spacing;
    std::vector<float> m_heightData;

    GLuint m_VAO, m_VBO, m_EBO;
    int m_indexCount;
};

#pragma once
#include "SceneNode.h"
#include <vector>

class SkyDomeNode : public SceneNode {
public:
    SkyDomeNode(float radius = 4500.0f, int rings = 32, int segments = 96);
    virtual ~SkyDomeNode();
    void update(float deltaTime) override;
    void draw(Renderer* renderer) override;  
    GLuint getVAO() const { return m_VAO; }
    int getIndexCount() const { return m_indexCount; }

private:
    void buildDome(float radius, int rings, int segments);

    struct Vertex {
        glm::vec3 position;
        glm::vec3 color;
    };
    std::vector<Vertex> m_vertices;
    std::vector<unsigned int> m_indices;

    GLuint m_VAO, m_VBO, m_EBO;
    int m_indexCount;
};

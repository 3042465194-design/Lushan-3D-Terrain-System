#pragma once

#include "SceneNode.h"
#include "STLLoader.h"   
#include <string>
#include <vector>

class PlaneNode : public SceneNode {
public:
    PlaneNode(const std::string& stlPath, float scale = 0.1f);  
    virtual ~PlaneNode();

    void update(float deltaTime) override;
    void draw(Renderer* renderer) override;

    void setPosition(const glm::vec3& pos) { m_position = pos; }
    glm::vec3 getPosition() const { return m_position; }
    int getVertexCount() const { return static_cast<int>(m_vertices.size()); }

private:
    void setupBuffers();

    std::vector<STLVertex> m_vertices;   
    std::vector<unsigned int> m_indices;
    GLuint m_VAO, m_VBO, m_EBO;
    glm::vec3 m_position;
    float m_rotationY;
};
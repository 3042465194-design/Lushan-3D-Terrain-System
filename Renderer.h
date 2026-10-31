#pragma once
#include "Common.h"
#include "ShaderManager.h"

class SceneNode;
class Camera;

class Renderer {
public:
    Renderer();
    ~Renderer();

    void initialize(int width, int height);
    void beginFrame();
    void endFrame();
    void setClearColor(float r, float g, float b, float a);
    void drawScene(SceneNode* root, Camera* camera, const ShaderManager& shader, float aspect);
    void resize(int width, int height);

private:
    int m_width, m_height;
    glm::vec4 m_clearColor;
};
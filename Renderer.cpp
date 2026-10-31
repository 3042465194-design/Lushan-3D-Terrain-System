#include "Renderer.h"
#include "SceneNode.h"
#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>

Renderer::Renderer() : m_width(800), m_height(600), m_clearColor(0.1f, 0.1f, 0.15f, 1.0f) {}
Renderer::~Renderer() {}

void Renderer::initialize(int width, int height) {
    m_width = width;
    m_height = height;
    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);
}

void Renderer::beginFrame() {
    glClearColor(m_clearColor.r, m_clearColor.g, m_clearColor.b, m_clearColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::endFrame() {
    // Пе
}

void Renderer::setClearColor(float r, float g, float b, float a) {
    m_clearColor = glm::vec4(r, g, b, a);
}

void Renderer::drawScene(SceneNode* root, Camera* camera, const ShaderManager& shader, float aspect) {
    shader.use();
    shader.setMat4("view", camera->getViewMatrix());
    shader.setMat4("projection", camera->getProjectionMatrix(aspect));
    shader.setMat4("model", glm::mat4(1.0f));
    root->draw(this);
}

void Renderer::resize(int width, int height) {
    m_width = width;
    m_height = height;
    glViewport(0, 0, width, height);
}

#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>

class SimpleMinimap {
public:
    SimpleMinimap();
    ~SimpleMinimap();

    void setBounds(const glm::vec2& min, const glm::vec2& max);
    void setRect(float x, float y, float width, float height);
    void setBackgroundTexture(GLuint texID);   // 可选底图纹理
    void draw(int screenWidth, int screenHeight, const glm::vec3& currentPos);
    void setVisible(bool visible) { m_visible = visible; }

private:
    void initBuffers();
    void drawBackground();
    void drawCurrentPoint(const glm::vec2& screenPos);
    glm::vec2 worldToScreen(const glm::vec2& world) const;

    bool m_visible;
    glm::vec4 m_rect;               // x, y, w, h (屏幕坐标)
    glm::vec2 m_worldMin, m_worldMax;
    GLuint m_vao, m_vbo, m_shader;
    GLuint m_orthoLoc, m_colorLoc, m_texLoc, m_useTexLoc;
    GLuint m_backgroundTexture;
    bool m_useTexture;
};

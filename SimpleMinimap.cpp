#include <glad/glad.h>
#include "SimpleMinimap.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

static const char* vertSrc = R"(
#version 330 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aTexCoord;
out vec2 TexCoord;
uniform mat4 projection;
void main() {
    TexCoord = aTexCoord;
    gl_Position = projection * vec4(aPos, 0.0, 1.0);
}
)";

static const char* fragSrc = R"(
#version 330 core
in vec2 TexCoord;
out vec4 FragColor;
uniform sampler2D uTexture;
uniform vec3 uColor;
uniform bool useTexture;
void main() {
    if (useTexture)
        FragColor = texture(uTexture, TexCoord);
    else
        FragColor = vec4(uColor, 1.0);
}
)";

SimpleMinimap::SimpleMinimap()
    : m_visible(true), m_rect(20, 20, 200, 150),
    m_worldMin(-5000, -5000), m_worldMax(5000, 5000),
    m_vao(0), m_vbo(0), m_shader(0), m_backgroundTexture(0), m_useTexture(false) {
    initBuffers();
}

SimpleMinimap::~SimpleMinimap() {
    if (m_vao) glDeleteVertexArrays(1, &m_vao);
    if (m_vbo) glDeleteBuffers(1, &m_vbo);
    if (m_shader) glDeleteProgram(m_shader);
}

void SimpleMinimap::initBuffers() {
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertSrc, nullptr);
    glCompileShader(vs);
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragSrc, nullptr);
    glCompileShader(fs);
    m_shader = glCreateProgram();
    glAttachShader(m_shader, vs);
    glAttachShader(m_shader, fs);
    glLinkProgram(m_shader);
    glDeleteShader(vs);
    glDeleteShader(fs);

    m_orthoLoc = glGetUniformLocation(m_shader, "projection");
    m_colorLoc = glGetUniformLocation(m_shader, "uColor");
    m_texLoc = glGetUniformLocation(m_shader, "uTexture");
    m_useTexLoc = glGetUniformLocation(m_shader, "useTexture");

    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    // 顶点格式: pos (2f) + texCoord (2f)
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
}

void SimpleMinimap::setBounds(const glm::vec2& min, const glm::vec2& max) {
    m_worldMin = min;
    m_worldMax = max;
}

void SimpleMinimap::setRect(float x, float y, float w, float h) {
    m_rect = glm::vec4(x, y, w, h);
}

void SimpleMinimap::setBackgroundTexture(GLuint texID) {
    m_backgroundTexture = texID;
    m_useTexture = (texID != 0);
}

void SimpleMinimap::drawBackground() {
    float vertices[] = {
        m_rect.x, m_rect.y,               0,0,
        m_rect.x + m_rect.z, m_rect.y,    1,0,
        m_rect.x, m_rect.y + m_rect.w,    0,1,
        m_rect.x + m_rect.z, m_rect.y + m_rect.w, 1,1
    };
    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

    glUseProgram(m_shader);
    if (m_useTexture) {
        glUniform1i(m_useTexLoc, 1);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_backgroundTexture);
        glUniform1i(m_texLoc, 0);
    }
    else {
        glUniform1i(m_useTexLoc, 0);
        glUniform3f(m_colorLoc, 0.0f, 0.0f, 0.0f); // 黑色背景
    }
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

glm::vec2 SimpleMinimap::worldToScreen(const glm::vec2& world) const {
    float u = (world.x - m_worldMin.x) / (m_worldMax.x - m_worldMin.x);
    float v = (world.y - m_worldMin.y) / (m_worldMax.y - m_worldMin.y);
    u = glm::clamp(u, 0.0f, 1.0f);
    v = glm::clamp(v, 0.0f, 1.0f);
    float sx = m_rect.x + u * m_rect.z;
    float sy = m_rect.y + (1.0f - v) * m_rect.w;
    return glm::vec2(sx, sy);
}

void SimpleMinimap::drawCurrentPoint(const glm::vec2& screenPos) {
    glUseProgram(m_shader);
    glUniform1i(m_useTexLoc, 0);
    glUniform3f(m_colorLoc, 1.0f, 0.2f, 0.2f); // 红色
    glPointSize(8.0f);
    float point[] = { screenPos.x, screenPos.y };
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(point), point, GL_DYNAMIC_DRAW);
    glDrawArrays(GL_POINTS, 0, 1);
}

void SimpleMinimap::draw(int screenWidth, int screenHeight, const glm::vec3& currentPos) {
    if (!m_visible) return;

    GLboolean depthTest = glIsEnabled(GL_DEPTH_TEST);
    GLboolean blend = glIsEnabled(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glm::mat4 ortho = glm::ortho(0.0f, (float)screenWidth, (float)screenHeight, 0.0f, -1.0f, 1.0f);
    glUseProgram(m_shader);
    glUniformMatrix4fv(m_orthoLoc, 1, GL_FALSE, glm::value_ptr(ortho));

    drawBackground();

    glm::vec2 curScreen = worldToScreen(glm::vec2(currentPos.x, currentPos.z));
    drawCurrentPoint(curScreen);

    if (depthTest) glEnable(GL_DEPTH_TEST);
    if (!blend) glDisable(GL_BLEND);
}
#pragma once
#include "Common.h"

class Camera;

class InputHandler {
public:
    InputHandler(GLFWwindow* window, Camera* camera);
    void processInput(float deltaTime);
    static void mouseCallback(GLFWwindow* window, double xpos, double ypos);
    static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);

private:
    GLFWwindow* m_window;
    Camera* m_camera;
    static InputHandler* s_instance;
    float m_lastX, m_lastY;
    bool m_firstMouse;
};

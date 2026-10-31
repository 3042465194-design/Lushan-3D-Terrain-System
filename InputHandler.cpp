#include "InputHandler.h"
#include "Camera.h"

InputHandler* InputHandler::s_instance = nullptr;

InputHandler::InputHandler(GLFWwindow* window, Camera* camera)
    : m_window(window), m_camera(camera), m_firstMouse(true) {
    s_instance = this;
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetScrollCallback(window, scrollCallback);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void InputHandler::processInput(float deltaTime) {
    if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(m_window, true);
    if (glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS)
        m_camera->processKeyboard(0, deltaTime);
    if (glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS)
        m_camera->processKeyboard(1, deltaTime);
    if (glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS)
        m_camera->processKeyboard(2, deltaTime);
    if (glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS)
        m_camera->processKeyboard(3, deltaTime);
}

void InputHandler::mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    if (s_instance->m_firstMouse) {
        s_instance->m_lastX = xpos;
        s_instance->m_lastY = ypos;
        s_instance->m_firstMouse = false;
    }
    float xoffset = xpos - s_instance->m_lastX;
    float yoffset = s_instance->m_lastY - ypos;
    s_instance->m_lastX = xpos;
    s_instance->m_lastY = ypos;
    s_instance->m_camera->processMouseMovement(xoffset, yoffset);
}

void InputHandler::scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    s_instance->m_camera->processMouseScroll(yoffset);
}

void InputHandler::framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}
#include "Camera.h"

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
    : m_position(position), m_worldUp(up), m_yaw(yaw), m_pitch(pitch),
    m_movementSpeed(5000.0f), m_mouseSensitivity(0.1f), m_zoom(45.0f), 
    m_orbitDistance(5000.0f), m_rotX(45.0f), m_rotY(30.0f)
{
    updateVectors();
    updateOrbitMatrix();
}

glm::mat4 Camera::getViewMatrix() const {
    return glm::lookAt(m_position, m_position + m_front, m_up);
}

glm::mat4 Camera::getProjectionMatrix(float aspectRatio) const {
    return glm::perspective(glm::radians(m_zoom), aspectRatio, 0.1f, 50000.0f);
}

void Camera::processKeyboard(int direction, float deltaTime) {
    float velocity = m_movementSpeed * deltaTime;
    if (direction == 0) // forward
        m_position += m_front * velocity;
    if (direction == 1) // backward
        m_position -= m_front * velocity;
    if (direction == 2) // left
        m_position -= m_right * velocity;
    if (direction == 3) // right
        m_position += m_right * velocity;
}

void Camera::processMouseMovement(float xoffset, float yoffset, bool constrainPitch) {
    xoffset *= m_mouseSensitivity;
    yoffset *= m_mouseSensitivity;

    m_yaw += xoffset;
    m_pitch += yoffset;

    if (constrainPitch) {
        if (m_pitch > 89.0f) m_pitch = 89.0f;
        if (m_pitch < -89.0f) m_pitch = -89.0f;
    }
    updateVectors();
}

void Camera::processMouseScroll(float yoffset) {
    m_zoom -= yoffset;
    if (m_zoom < 1.0f) m_zoom = 1.0f;
    if (m_zoom > 45.0f) m_zoom = 45.0f;
}

void Camera::updateVectors() {
    glm::vec3 front;
    front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    front.y = sin(glm::radians(m_pitch));
    front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    m_front = glm::normalize(front);
    m_right = glm::normalize(glm::cross(m_front, m_worldUp));
    m_up = glm::normalize(glm::cross(m_right, m_front));
}
void Camera::setOrbit(float distance, float rotX, float rotY) {
    m_orbitDistance = distance;
    m_rotX = rotX;
    m_rotY = rotY;
    updateOrbitMatrix();
}

void Camera::updateOrbitMatrix() {
    float radX = glm::radians(m_rotX);
    float radY = glm::radians(m_rotY);
    float cx = m_orbitDistance * cos(radY) * cos(radX);
    float cy = m_orbitDistance * sin(radY);
    float cz = m_orbitDistance * cos(radY) * sin(radX);
    m_position = glm::vec3(cx, cy, cz);
    m_front = glm::normalize(glm::vec3(0.0f, 0.0f, 0.0f) - m_position);
    m_right = glm::normalize(glm::cross(m_front, m_worldUp));
    m_up = glm::normalize(glm::cross(m_right, m_front));
}

void Camera::mouseDrag(float dx, float dy) {
    m_rotX += dx * 0.5f;
    m_rotY += dy * 0.5f;
    if (m_rotY > 89.0f) m_rotY = 89.0f;
    if (m_rotY < -89.0f) m_rotY = -89.0f;
    updateOrbitMatrix();
}

void Camera::zoom(float delta) {
    m_orbitDistance -= delta * 100.0f;
    if (m_orbitDistance < 1000.0f) m_orbitDistance = 1000.0f;
    updateOrbitMatrix();
}

void Camera::setFirstPerson(const glm::vec3& position, const glm::vec3& front, const glm::vec3& up) {
    m_position = position;
    m_front = glm::normalize(front);
    m_worldUp = up;
    m_right = glm::normalize(glm::cross(m_front, m_worldUp));
    m_up = glm::normalize(glm::cross(m_right, m_front));
}

glm::vec3 Camera::getFront() const { return m_front; }
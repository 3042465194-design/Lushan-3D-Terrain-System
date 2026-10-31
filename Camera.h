#pragma once
#include "Common.h"

class Camera {
public:
    Camera(glm::vec3 position = glm::vec3(0.0f, 10.0f, 20.0f),
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
        float yaw = -90.0f, float pitch = 0.0f);

    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix(float aspectRatio) const;

    void processKeyboard(int direction, float deltaTime);
    void processMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);
    void processMouseScroll(float yoffset);
    void setPosition(const glm::vec3& pos) { m_position = pos; }

    void setOrbit(float distance, float rotX, float rotY);
    void updateOrbitMatrix();
    void mouseDrag(float dx, float dy);
    void zoom(float delta);
    glm::vec3 getPosition() const { return m_position; }
    void setFirstPerson(const glm::vec3& position, const glm::vec3& front, const glm::vec3& up = glm::vec3(0.0f, 1.0f, 0.0f));
    glm::vec3 getFront() const;
private:
    void updateVectors();

    glm::vec3 m_position;
    glm::vec3 m_front;
    glm::vec3 m_up;
    glm::vec3 m_right;
    glm::vec3 m_worldUp;

    float m_yaw;
    float m_pitch;
    float m_movementSpeed;
    float m_mouseSensitivity;
    float m_zoom;
    float m_orbitDistance;
    float m_rotX;
    float m_rotY;
};


#include "../headers/Camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


Camera::Camera(const glm::vec3& startPosition,
               const glm::vec3& startUp,
               float startYaw,
               float startPitch)
    : position(startPosition),
      worldUp(startUp),
      up(startUp),
      yaw(startYaw),
      pitch(startPitch),
      movementSpeed(200.0f),
      mouseSensitivity(0.1f)
{
    updateCameraVectors();
}

glm::mat4 Camera::getViewMatrix() const
{
    return glm::lookAt(position, position + front, up);
}

glm::vec3 Camera::getCamPosition() const
{
    return position;
}

void Camera::setCamPosition(glm::vec3 val)
{
    position = val;
}

void Camera::setCamSpeed(float newSpeed)
{
    movementSpeed = newSpeed;
}

void Camera::processKeyboardForward(float deltaTime)
{
    float velocity = movementSpeed * deltaTime;
    position += front * velocity;
}

void Camera::processKeyboardBackward(float deltaTime)
{
    float velocity = movementSpeed * deltaTime;
    position -= front * velocity;
}

void Camera::processKeyboardLeft(float deltaTime)
{
    float velocity = movementSpeed * deltaTime;
    position -= right * velocity;
}

void Camera::processKeyboardRight(float deltaTime)
{
    float velocity = movementSpeed * deltaTime;
    position += right * velocity;
}

void Camera::processMouseMovement(float xOffset, float yOffset)
{
    xOffset *= mouseSensitivity;
    yOffset *= mouseSensitivity;

    yaw   += xOffset;
    pitch -= yOffset;

    if (pitch > 89.0f)  pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    updateCameraVectors();
}

void Camera::updateCameraVectors()
{
    glm::vec3 newFront;
    newFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    newFront.y = sin(glm::radians(pitch));
    newFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(newFront);

    right = glm::normalize(glm::cross(front, worldUp)); 
    up    = glm::normalize(glm::cross(right, front));  // This code does not change anything learn why because it seems necessary
}  
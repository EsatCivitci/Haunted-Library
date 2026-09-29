#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
    Camera (const glm::vec3& startPosition,
            const glm::vec3& startUp,
            float startYaw,
            float startPitch);

    glm::mat4 getViewMatrix() const;
    glm::vec3 getCamPosition() const;

    void setCamPosition(glm::vec3 val);
    void setCamSpeed(float newSpeed);

    void processKeyboardForward(float deltaTime);
    void processKeyboardBackward(float deltaTime);
    void processKeyboardLeft(float deltaTime);
    void processKeyboardRight(float deltaTime);
    void processMouseMovement(float xOffset, float yOffset);

    glm::vec3 getPosition() const {return position;}

private:
    glm::vec3 position;
    glm::vec3 up;
    glm::vec3 front;
    glm::vec3 right;
    glm::vec3 worldUp;

    // Euler Angles
    float yaw;   // Left-right rotation
    float pitch; // Up-down rotation

    float movementSpeed;
    float mouseSensitivity;

    void updateCameraVectors();

};

#endif 
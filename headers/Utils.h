#ifndef UTILS_H
#define UTILS_H

#include "Camera.h"
#include "Shader.h"
#include "Path.h"
#include "Ghost.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <string>

void generatePath();
void renderGhost();

void printVec3(const std::string& name, const glm::vec3& vec);

GLuint loadTexture(const char* filename);

void utilizeGhostShader(Shader* shader, const glm::mat4& model);
void utilizePathShader(Shader* shader);
void utilizePathShader(Shader* shader);

float calculateDeltaTime();
void processInput(GLFWwindow* window, Camera &camera);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

#endif
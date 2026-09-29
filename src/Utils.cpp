#include "../headers/Utils.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../headers/stb_image.h"

#include <iostream>
#include <vector>
#include <random>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

using namespace std;

extern Camera* camera;
extern Path* path;
extern Shader* ghostShader;
extern Ghost* ghost;

//extern glm::mat4 model;
extern glm::mat4 view;
extern glm::mat4 projection;

extern int gWidth;          
extern int gHeight;   
extern int nSamples;

extern bool pauseFlag;

extern glm::vec3 CP[4];

glm::vec3 prevUp = {0.0f, 1.0f, 0.0f};

float currentFrame = 0.0f;
float lastFrame = 0.0f;

float currTime = 0;
float totalTAmount = 0;

glm::vec3 P0;
glm::vec3 P1;
glm::vec3 addP;
glm::vec3 tangent;
glm::vec3 tangent0;
glm::vec3 tangent1;


int prevIndex = -1;
float sumDelta = 0;

void printVec3(const std::string& name, const glm::vec3& vec) {
    std::cout << name << " = (" 
              << vec.x << ", " 
              << vec.y << ", " 
              << vec.z << ")\n";
}

void renderGhost() {
    
    float delta = calculateDeltaTime();
    
    if (!pauseFlag) {
        float tAmount =  delta * 4.0f/ nSamples;
        totalTAmount += tAmount;
        if (totalTAmount > (4.0f / nSamples)) {
            totalTAmount = 0;
        }

        currTime+=delta;
        if (currTime > 4){
            prevIndex = -1;
            currTime = 0;
            generatePath();
        }
    }
    
    float t = currTime / 4.0f;

    float pathT = t * (nSamples - 1);
    int index = std::min((int)pathT, nSamples - 2);
    float localT = pathT - index;

    P0 = glm::vec3(
        path->getVertices()[index * 3 + 0],
        path->getVertices()[index * 3 + 1],
        path->getVertices()[index * 3 + 2]
    );
    P1 = glm::vec3(
        path->getVertices()[(index + 1) * 3 + 0],
        path->getVertices()[(index + 1) * 3 + 1],
        path->getVertices()[(index + 1) * 3 + 2]
    );
    glm::vec3 tangent0 = glm::normalize(glm::vec3(
        path->getDerivatives()[index * 3 + 0],
        path->getDerivatives()[index * 3 + 1],
        path->getDerivatives()[index * 3 + 2]
    ));
    glm::vec3 tangent1 = glm::normalize(glm::vec3(
        path->getDerivatives()[(index + 1) * 3 + 0],
        path->getDerivatives()[(index + 1) * 3 + 1],
        path->getDerivatives()[(index + 1) * 3 + 2]
    ));

    glm::vec3 interpolatedPosition = glm::mix(P0, P1, localT);
    glm::vec3 tangent = glm::normalize(glm::mix(tangent0, tangent1, localT));

    glm::vec3 front = tangent;
    glm::vec3 right   = glm::normalize(glm::cross(prevUp, front));
    glm::vec3 up = glm::normalize(glm::cross(front, right));
    prevUp = up;  
    
    glm::mat4 orientation = glm::mat4(1.0f);
    orientation[0] = glm::vec4(right, 0.0f); 
    orientation[1] = glm::vec4(up, 0.0f); 
    orientation[2] = glm::vec4(front, 0.0f); 

    static float rollDeg = 0;
    static float changeRoll = 2.5;
    float rollRad = (float)(rollDeg / 180.f) * M_PI;

    if (!pauseFlag) {
        rollDeg += changeRoll;
        if (rollDeg >= 30.f || rollDeg <= -30.f)
        {
            changeRoll *= -1.f;
        }
    }
    glm::quat rollQuat  = glm::angleAxis(rollRad,  front); 

    glm::mat4 model = glm::toMat4(rollQuat) * orientation; 

    model = glm::translate(glm::mat4(1.0f), interpolatedPosition) * model;
    
    model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 1, 0));
    model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0));
    model = glm::scale(model, glm::vec3(0.5f));  
    
    utilizeGhostShader(ghostShader, model);
    ghost->draw();
    
}


void generatePath() {
    float boundX = 15.0f;
    float boundY = 8.0f;
    float boundZ = 8.0f;
    std::random_device rDevice;
    std::mt19937 gen(rDevice());
    std::uniform_real_distribution<float> distrX(-boundX, boundX);
    std::uniform_real_distribution<float> distrY(-boundY, boundY);
    std::uniform_real_distribution<float> distrZ(-boundZ, boundZ);

    glm::vec3 CP1 = CP[3];

    glm::vec3 CP2 = ((CP[3]-CP[2])) + CP[3];
    
    float randomX1 = distrX(gen);
    float randomY1 = distrY(gen);
    float randomZ1 = distrZ(gen);

    glm::vec3 CP3 = {randomX1, randomY1, randomZ1};

    float randomX2 = distrX(gen);
    float randomY2 = distrY(gen);
    float randomZ2 = distrZ(gen);

    glm::vec3 CP4 = {randomX2, randomY2, randomZ2};

    CP[0] = CP1;
    CP[1] = CP2;
    CP[2] = CP3;
    CP[3] = CP4;

    path->~Path();
    path = new Path(nSamples, CP);
}

void utilizeGhostShader(Shader* shader, const glm::mat4& model) {
    view = camera->getViewMatrix();
    projection = glm::perspective(glm::radians(45.0f), (float)gWidth/gHeight, 0.1f, 100.0f);
    
    shader->use();

    shader->setMat4("modelingMatrix", model);
    shader->setMat4("viewingMatrix", view);
    shader->setMat4("projectionMatrix", projection);
    shader->setVec3("eyePos", camera->getPosition());
}

void utilizePathShader(Shader* shader) {
    glm::mat4 modelP = glm::mat4(1.0f);
    glm::mat4 viewP = camera->getViewMatrix();
    glm::mat4 projectionP = glm::perspective(glm::radians(45.0f), (float)gWidth/gHeight, 0.1f, 100.0f);
    
    shader->use();

    shader->setMat4("modelingMatrix", modelP);
    shader->setMat4("viewingMatrix", viewP);
    shader->setMat4("projectionMatrix", projectionP);
}

float calculateDeltaTime(){
    currentFrame = glfwGetTime();
    float deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    return deltaTime;
}


GLuint loadTexture(const char* filename) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int widht, height, nrChannels;
    unsigned char* img = stbi_load(filename, &widht, &height, &nrChannels, 0);

    if (img){
        GLenum format = (nrChannels == 3) ? GL_RGB : GL_RGBA;
        glTexImage2D(GL_TEXTURE_2D, 0, format, widht, height, 0, format, GL_UNSIGNED_BYTE, img);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else{
        std::cerr << "Failed to load texture: " << filename << std::endl;
    }
    stbi_image_free(img);

    return textureID;
}


void processInput(GLFWwindow* window, Camera &camera) {
    float deltaTime = calculateDeltaTime();

    // Close window
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // Reset cam position
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.setCamPosition(glm::vec3(0.0f, 0.0f, 40.0f));
    
    // Cam movement
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.processKeyboardForward(deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.processKeyboardBackward(deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.processKeyboardLeft(deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.processKeyboardRight(deltaTime);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {

    static float lastX = gWidth / 2.0f;
    static float lastY = gHeight / 2.0f;
    
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; 
    
    lastX = xpos;
    lastY = ypos;
    
    camera->processMouseMovement(xoffset, yoffset);
}
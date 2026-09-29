#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include <iostream>
#include <string>
#include <vector>

#include "../headers/Shader.h"
#include "../headers/Utils.h"
#include "../headers/Quad.h"
#include "../headers/Camera.h"
#include "../headers/Ghost.h"
#include "../headers/Path.h"

using namespace std;

// ************** FUNCTION DECLERATIONS ***************
void init();
void display();
void reshape(GLFWwindow* window, int width, int height);
void keyboard(GLFWwindow* window, int key, int scancode, int action, int mods);

// ************** VARIABLES ****************
GLFWwindow* window;
int gWidth = 1920, gHeight = 1080;

Camera* camera;

Shader* quadShader;
Quad* quadObject;   

Ghost* ghost;
Shader* ghostShader;

Path* path;
Shader* pathShader;

GLuint quadTexture;

glm::mat4 model = glm::mat4(1.0f);
glm::mat4 view;
glm::mat4 projection;

glm::vec3 camPos = {0.0f, 0.0f, 50.0f};
glm::vec3 camUp = {0.0f, 1.0f, 0.0f};
float yawAngle = -90.0f;
float pitchAngle = 0.0f;

bool pathFlag = false;
bool pauseFlag = false;
bool switchFlag = false;


// float currentFrame = 0;
// float lastFrame = 0;

glm::vec3 CP[4] = {
    {-2,0,0},{-1,0,0},{1,0,0},{2,0,0}
};

int nSamples = 12;


int main(int argc, char* argv[]) {
    if (argc >= 2) {
        nSamples = std::atoi(argv[1]);
    }

    if (!glfwInit()) {
        cerr << "Failed to initialize GLFW!!" << endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    window = glfwCreateWindow(gWidth, gHeight, "THE1", NULL, NULL);
    if (!window) {
        cerr << "Failed to create window!!" << endl;
        glfwTerminate();
		exit(-1);
    }

    glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

    if (glewInit() != GLEW_OK){
        cerr << "Failed to initialize GLEW!!" << endl;
        return -1;
    }

    init();

	glfwSetKeyCallback(window, keyboard);  // Implement keyboard function
    glfwSetWindowUserPointer(window, &camera);
    glfwSetFramebufferSizeCallback(window, reshape);

    // glfwSetCursorPosCallback(window, mouse_callback);
    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // Lock cursor to the scene

    while (!glfwWindowShouldClose(window))
	{
        //processInput(window, *camera);

		display();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

    glfwDestroyWindow(window);
	glfwTerminate();
    return 0;
}

void init() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);

    camera = new Camera(
        camPos,  
        camUp,  
        yawAngle,                      
        pitchAngle                        
    );

    quadShader = new Shader("shaders/vert_quad.glsl", "shaders/frag_quad.glsl");
    quadObject = new Quad("textures/quad.obj");

    ghost = new Ghost(nSamples);
    ghostShader = new Shader("shaders/vert_ghost.glsl", "shaders/frag_ghost.glsl");

    path = new Path(nSamples, CP);
    pathShader = new Shader("shaders/vert_path.glsl", "shaders/frag_path.glsl");

    generatePath();

    quadTexture = loadTexture("textures/haunted_library.jpg");
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glDepthMask(GL_FALSE);
    quadShader->use();
    quadShader->setFloat("ourTexture", quadTexture);
    quadObject->draw();
    glDepthMask(GL_TRUE);

    if (switchFlag) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    renderGhost();

    utilizePathShader(pathShader);
    if (pathFlag) path->draw();
}

void keyboard(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if ((key == GLFW_KEY_ESCAPE) && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
    if ((key == GLFW_KEY_P) && action == GLFW_PRESS)
    {
        if (pathFlag) pathFlag = false;
        else pathFlag = true;
    }
    if ((key == GLFW_KEY_SPACE) && action == GLFW_PRESS)
    {
        if (pauseFlag) pauseFlag = false;
        else pauseFlag = true;
    }
    if ((key == GLFW_KEY_W) && action == GLFW_PRESS)
    {
        if (switchFlag) switchFlag = false;
        else switchFlag = true;
    }
}

void reshape(GLFWwindow* window, int width, int height){
    gWidth = width;
    gHeight = height;
    glViewport(0, 0, width, height);
}


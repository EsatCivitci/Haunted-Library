#ifndef PATH_H
#define PATH_H

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <vector>

class Path {
public:
    Path(int n, glm::vec3* inputCP);
    ~Path();

    std::vector<float> getVertices();
    std::vector<float> getDerivatives();

    void initControlPoints();
    void createBezierPoints(float t);

    void initVBO();
    void draw();

private:
    GLuint VAO, VBO, EBO;

    glm::vec3 CP[4];

    int nSamples;
    std::vector<float> vertices;
    std::vector<int> indices;
    std::vector<float> derivatives;

};

#endif
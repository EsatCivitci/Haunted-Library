#include "../headers/Path.h"

#include <iostream>

Path::Path(int n, glm::vec3* inputCP) : nSamples(n){

    for (int i = 0; i < 4; ++i) {
        CP[i] = inputCP[i]; 
    }

    initControlPoints();
}

Path::~Path() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

std::vector<float> Path::getVertices() {
    return vertices;
}

std::vector<float> Path::getDerivatives() {
    return derivatives;
}

void Path::initControlPoints() {
    for (int i = 0; i < nSamples; ++i) {
            float t = (float)i / (nSamples-1);
            createBezierPoints(t);
            indices.push_back(i);
    }

    initVBO();
}

void Path::createBezierPoints(float t) {
    float bernsteinS[4], bernsteinT[4];
    float derBernsteinS[4], derBernsteinT[4];

    float pointX = 0, pointY = 0, pointZ = 0;
    float dtX = 0, dtY = 0, dtZ = 0;

    bernsteinT[0] = pow((1-t),3);
    bernsteinT[1] = 3*pow((1-t),2) * (t);
    bernsteinT[2] = 3*(1-t) * pow(t,2);
    bernsteinT[3] = pow(t,3);

    derBernsteinT[0] = -3 * pow((1 - t), 2);
    derBernsteinT[1] = -3 * 2 * (1 - t) * t + 3 * pow((1 - t), 2);
    derBernsteinT[2] = -3 * pow(t, 2) + 3 * (1 - t) * 2 * t;
    derBernsteinT[3] = 3 * pow(t, 2); 

    for (int i = 0; i < 4; ++i) {
            pointX += bernsteinT[i] * CP[i].x;
            pointY += bernsteinT[i] * CP[i].y;
            pointZ += bernsteinT[i] * CP[i].z;

            dtX += derBernsteinT[i] * CP[i].x;
            dtY += derBernsteinT[i] * CP[i].y;
            dtZ += derBernsteinT[i] * CP[i].z;
    }

    vertices.push_back(pointX);
    vertices.push_back(pointY);
    vertices.push_back(pointZ);

    derivatives.push_back(dtX);
    derivatives.push_back(dtY);
    derivatives.push_back(dtZ);

    //std::cout << "X: " << pointX << " Y: " << pointY << " Z: " << pointZ << std::endl;

}

void Path::initVBO() {
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

void Path::draw() {
    glBindVertexArray(VAO);
    glDrawElements(GL_LINE_STRIP,  indices.size(), GL_UNSIGNED_INT, (void*)0);
    glBindVertexArray(0);
}
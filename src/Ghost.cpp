#include "../headers/Ghost.h"

#include <string>
#include <math.h>
#include <GL/glew.h>


Ghost::Ghost(int n) : nSamples(n) {
    initControlPoints();
}

Ghost::~Ghost() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void printVerNormals(const std::vector<float>& verNormals) {
    for (size_t i = 0; i < verNormals.size(); i += 3) {
        if (i + 2 < verNormals.size()) {
            std::cout << verNormals[i] << " " 
                      << verNormals[i + 1] << " " 
                      << verNormals[i + 2] << std::endl;
        } else {
            for (size_t j = i; j < verNormals.size(); ++j) {
                std::cout << verNormals[j] << " ";
            }
            std::cout << std::endl;
        }
    }
}

void Ghost::initControlPoints() {
    float t,s;
    int collumn = 0;
    int row  = 0;

    for (int n = 0; n < nSurf; ++n) {
        for (int i = 0; i < nSamples; ++i){
            collumn += nSamples;
            t = (float)i / (nSamples-1);
            for (int j = 0; j < nSamples; ++j, ++row) {
                s = (float)j / (nSamples-1);
                createBezierPoints(s, t, n);
                if (i == nSamples-1) continue;
                indices.push_back(row);
                indices.push_back(collumn+j);
            }
        }
    }

    collumn = 0;
    int surfIndex = 0;
    int indicesSize = indices.size();
    for (int n = 0; n < nSurf; ++n){
        for (int i = 0; i < nSamples-1; ++i){
            for (int j = 0; j < 2*(nSamples-1);) {
                glm::vec3 p1,p2,p3,p4;
                //std::cout << "j: " << j << "   collumn: " << collumn << "   surfIndex: " << surfIndex << std::endl;
                int indexV1 = indices[j+collumn + surfIndex];
                int indexV2 = indices[j+collumn + surfIndex]+3;
                int indexV3 = indices[j+collumn + surfIndex]+6;
                int indexV4 = indices[j+collumn + surfIndex]+9;

                p1.x = vertices[3*indices[j+collumn + surfIndex]];
                p1.y = vertices[3*indices[j+collumn + surfIndex]+1];
                p1.z = vertices[3*indices[j+collumn + surfIndex]+2];

                p2.x = vertices[3*indices[j+collumn + surfIndex+1]];
                p2.y = vertices[3*indices[j+collumn + surfIndex+1]+1];
                p2.z = vertices[3*indices[j+collumn + surfIndex+1]+2];

                p3.x = vertices[3*indices[j+collumn + surfIndex+2]];
                p3.y = vertices[3*indices[j+collumn + surfIndex+2]+1];
                p3.z = vertices[3*indices[j+collumn + surfIndex+2]+2];

                p4.x = vertices[3*indices[j+collumn + surfIndex+3]];
                p4.y = vertices[3*indices[j+collumn + surfIndex+3]+1];
                p4.z = vertices[3*indices[j+collumn + surfIndex+3]+2];

                glm::vec3 normal1,normal2;

                normal1 = glm::cross(p1-p2,p3-p2);
                normal2 = glm::cross(p4-p3,p2-p3);

                normalizeZero(normal1);
                normalizeZero(normal2);

                if (!(normal1.x == 0 && normal1.y == 0 && normal1.z == 0)){
                    normal1 = glm::normalize(normal1);
                }
                if (!(normal2.x == 0 && normal2.y == 0 && normal2.z == 0)){
                    normal2 = glm::normalize(normal2);
                }

                glm::vec3 prevNormal;
                if (triangleNormals.size() > 0)
                    prevNormal = triangleNormals.back();

                if (n == 6 || n == 5){  
                    if (normal1.x >= 0){
                        normal1 = prevNormal;
                    }                    
                    if (normal2.x >= 0){
                        normal2 = prevNormal;
                    }
                }

                if (n == 7 || n == 4){    
                    if (normal1.x <= 0){
                        normal1 = prevNormal;
                    }                    
                    if (normal2.x <= 0){
                        normal2 = prevNormal;
                    }
                }
                if (n >= 16) {
                    if (normal1.z == 0) {
                        normal1 = glm::vec3 (0.0f,0.0f,-1.0f);
                    }
                    if (normal2.z == 0) {
                        normal2 = glm::vec3 (0.0f,0.0f,-1.0f);;
                    }
                }
                else if (n >= 12 ) {
                    if (normal1.z == 0) {
                        normal1 = prevNormal;
                    }
                    if (normal2.z == 0) {
                        normal2 = prevNormal;
                    }
                }

                triangleNormals.push_back(normal1);
                triangleNormals.push_back(normal2);

                j+=2;
            }
            //std::cout << std::endl;
            collumn+=2*nSamples;
        }
        surfIndex += collumn;
        collumn = 0;
    }

    surfIndex = 0;
    collumn = 0;
    int tIndex = -2;
    int count = 0;
    for (int n = 0; n < nSurf; ++n) {
        for (int i = 0; i < nSamples; ++i) {
            for (int j = 0; j < nSamples; ++j) {
                glm::vec3 result;
                int tIndex2 = tIndex-(2*(nSamples-1))+1;


                if (i == 0 && j == 0) {
                    // std::cout << "1" << std::endl;
                    glm::vec3 n3 = triangleNormals[surfIndex + tIndex+2];
                    glm::vec3 n7 = triangleNormals[surfIndex + tIndex+3];

                    if (n3.x == 0 && n3.y == 0 && n3.z == 0)
                        result = (n3 + n7) * (1.0f / 2.0f);
                    else result = n3;
                }

                else if (i == 0 && j == (nSamples-1)){
                    // std::cout << "2" << std::endl;
                    glm::vec3 n1 = triangleNormals[surfIndex + tIndex];
                    glm::vec3 n2 = triangleNormals[surfIndex + tIndex+1];
                    result = (n1 + n2) * (1.0f / 2.0f);
                }

                else if (i == (nSamples-1) && j == 0) {
                    // std::cout << "3    t2: " << tIndex2 << std::endl;
                    glm::vec3 n5 = triangleNormals[surfIndex + tIndex2+1];
                    glm::vec3 n6 = triangleNormals[surfIndex + tIndex2+2];
                    result = (n5 + n6) * (1.0f / 2.0f);
                }

                else if ((i == (nSamples-1) && j == (nSamples-1))){
                    // std::cout << "4" << "    t2: " << tIndex2 << std::endl;
                    // std::cout << tIndex2<< std::endl;
                    glm::vec3 n4 = triangleNormals[surfIndex + tIndex2];
                    glm::vec3 n8 = triangleNormals[surfIndex + tIndex2-1];

                    result = (n4 + n8) * (1.0f / 2.0f);

                    if (n4.x == 0 && n4.y == 0 && n4.z == 0)
                        result = (n4 + n8) * (1.0f / 2.0f);
                    else result = n4;
                }

                else if (j == 0) {
                    // std::cout << "5" << "    t2: " << tIndex2 << std::endl;
                    glm::vec3 n3 = triangleNormals[surfIndex + tIndex+2];
                    glm::vec3 n5 = triangleNormals[surfIndex + tIndex2+1];
                    glm::vec3 n6 = triangleNormals[surfIndex + tIndex2+2];
                    result = (n3 + n5 + n6) * (1.0f / 3.0f);
                }

                else if (i == 0) {
                    // std::cout << "6" << std::endl;                
                    glm::vec3 n1 = triangleNormals[surfIndex + tIndex];
                    glm::vec3 n2 = triangleNormals[surfIndex + tIndex+1];
                    glm::vec3 n3 = triangleNormals[surfIndex + tIndex+2];
                    result = (n1 + n2 + n3) * (1.0f / 3.0f);
                }

                else if (j == (nSamples-1)){

                    glm::vec3 n1 = triangleNormals[surfIndex + tIndex];
                    glm::vec3 n2 = triangleNormals[surfIndex + tIndex+1];
                    glm::vec3 n4 = triangleNormals[surfIndex + tIndex2];
                    result = (n1 + n2 + n4) * (1.0f / 3.0f);
                }

                else if (i == (nSamples-1)){
                    glm::vec3 n4 = triangleNormals[surfIndex + tIndex2];
                    glm::vec3 n5 = triangleNormals[surfIndex + tIndex2+1];
                    glm::vec3 n6 = triangleNormals[surfIndex + tIndex2+2];
                    result = (n4 + n5 + n6) * (1.0f / 3.0f);
                }

                else {
                    glm::vec3 n1 = triangleNormals[surfIndex + tIndex];
                    glm::vec3 n2 = triangleNormals[surfIndex + tIndex+1];
                    glm::vec3 n3 = triangleNormals[surfIndex + tIndex+2];
    
                    glm::vec3 n4 = triangleNormals[surfIndex + tIndex2];
                    glm::vec3 n5 = triangleNormals[surfIndex + tIndex2+1];
                    glm::vec3 n6 = triangleNormals[surfIndex + tIndex2+2];
                    result = (n1 + n2 + n3 + n4 + n5 + n6) * (1.0f / 6.0f);
                }
                result = glm::normalize(result);

                
                //std::cout << result.x << "  " << result.y << "   " << result.z << std::endl;

                interpolatedNormals.push_back(result.x);
                interpolatedNormals.push_back(result.y);
                interpolatedNormals.push_back(result.z);

                // std::cout << "tIndex: " << tIndex << "   surfIndex: " << surfIndex << " index: " << surfIndex+tIndex << std::endl << std::endl;
                
                if (j < (nSamples-1)) {
                    tIndex += 2;
                }   
            }
            collumn+=2*(nSamples-1);
        }
        collumn = -2;
        // std::cout << std::endl;w
        surfIndex += 2*(nSamples-1)*(nSamples-1);
        tIndex = -2;
    }

    //  std::cout << interpolatedNormals.size()/3 << std::endl;
    

    initVBO();
}



void Ghost::createBezierPoints(float s, float t, int surfIndex){
    float bernsteinS[4], bernsteinT[4];
    float derBernsteinS[4], derBernsteinT[4];
    float pointX = 0, pointY = 0, pointZ = 0;

    float dsX = 0, dsY = 0, dsZ = 0;
    float dtX = 0, dtY = 0, dtZ = 0;
    float normalX, normalY, normalZ;

    bernsteinS[0] = pow((1-s),3);
    bernsteinS[1] = 3*pow((1-s),2) * (s);
    bernsteinS[2] = 3*(1-s) * pow(s,2);
    bernsteinS[3] = pow(s,3);

    bernsteinT[0] = pow((1-t),3);
    bernsteinT[1] = 3*pow((1-t),2) * (t);
    bernsteinT[2] = 3*(1-t) * pow(t,2);
    bernsteinT[3] = pow(t,3);

    derBernsteinS[0] = -3 * pow((1 - s), 2);
    derBernsteinS[1] = -3 * 2 * (1 - s) * s + 3 * pow((1 - s), 2);
    derBernsteinS[2] = -3 * pow(s, 2) + 3 * (1 - s) * 2 * s;
    derBernsteinS[3] = 3 * pow(s, 2);
    
    derBernsteinT[0] = -3 * pow((1 - t), 2);
    derBernsteinT[1] = -3 * 2 * (1 - t) * t + 3 * pow((1 - t), 2);
    derBernsteinT[2] = -3 * pow(t, 2) + 3 * (1 - t) * 2 * t;
    derBernsteinT[3] = 3 * pow(t, 2);    


    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            pointX += bernsteinS[i] * bernsteinT[j] * CP[surfIndex][i][j].x;
            pointY += bernsteinS[i] * bernsteinT[j] * CP[surfIndex][i][j].y;
            pointZ += bernsteinS[i] * bernsteinT[j] * CP[surfIndex][i][j].z;

            dsX += derBernsteinS[i] * bernsteinT[j] * CP[surfIndex][i][j].x;
            dsY += derBernsteinS[i] * bernsteinT[j] * CP[surfIndex][i][j].y;
            dsZ += derBernsteinS[i] * bernsteinT[j] * CP[surfIndex][i][j].z;
    
            dtX += bernsteinS[i] * derBernsteinT[j] * CP[surfIndex][i][j].x;
            dtY += bernsteinS[i] * derBernsteinT[j] * CP[surfIndex][i][j].y;
            dtZ += bernsteinS[i] * derBernsteinT[j] * CP[surfIndex][i][j].z;
        }
    }


    normalX = dsY * dtZ - dsZ * dtY;
    normalY = dsZ * dtX - dsX * dtZ;
    normalZ = dsX * dtY - dsY * dtX;

    float length = sqrt(normalX * normalX + normalY * normalY + normalZ * normalZ);
    if (length > 0.001) { 
        normalX /= length;
        normalY /= length;
        normalZ /= length;
    } else {
        normalX = 0;
        normalY = 0;
        normalZ = 1;
        if (surfIndex < 6) normalY = 1;
        else if (surfIndex < 8) normalY = -1;
        else if (surfIndex < 16) normalZ = 1;
        else if (surfIndex < 20) normalZ = -1;
    }

    vertices.push_back(pointX);
    vertices.push_back(pointY);
    vertices.push_back(pointZ);

    normals.push_back(normalX);
    normals.push_back(normalY);
    normals.push_back(normalZ);
}

void Ghost::initVBO() {
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //glBufferData(GL_ARRAY_BUFFER, (vertices.size() + normals.size()) * sizeof(float), NULL, GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, (vertices.size() + interpolatedNormals.size()) * sizeof(float), NULL, GL_STATIC_DRAW);

    glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(float), vertices.data());
    //glBufferSubData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), normals.size() * sizeof(float), normals.data()); 
    glBufferSubData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), interpolatedNormals.size() * sizeof(float), interpolatedNormals.data()); 

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(vertices.size() * sizeof(float)));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void Ghost::draw() {
    glBindVertexArray(VAO);
    for (int i = 1; i < nSamples*nSurf; ++i) {
        //if (i%nSamples == 0) continue;
        glDrawElements(GL_TRIANGLE_STRIP,  nSamples*2, GL_UNSIGNED_INT, (void*)((i-1)*sizeof(int)*nSamples*2));
        //glDrawElements(GL_LINE_STRIP,  nSamples*2, GL_UNSIGNED_INT, (void*)((i-1)*sizeof(int)*nSamples*2));
    }
    glBindVertexArray(0);
}

void Ghost::normalizeZero(glm::vec3 &vec) {
    vec.x = (vec.x == 0.0f) ? 0.0f : vec.x;
    vec.y = (vec.y == 0.0f) ? 0.0f : vec.y;
    vec.z = (vec.z == 0.0f) ? 0.0f : vec.z;
}

glm::vec3 Ghost::takeAverage(glm::vec3 vec1, glm::vec3 vec2) {
    return glm::normalize((vec1+vec2) * (1.0f)/(2.0f));
}

    
    // Initialize indices
    // for (int n = 0; n < nSurf; ++n) {
    //     for (int i = 0; i < nSamples; ++i) {
    //         collumn += nSamples;
    //         for (int j = 0; j < nSamples; ++j, ++row) {
    //             indices.push_back(row);
    //             indices.push_back(collumn+j);
    //         }
    //     }
    // }

    /*
    // Initialize normals
    for (int n = 0; n < nSurf; ++n) {
        for (int i = 0; i < nSamples; ++i) {
            for (int j = 0; j < nSamples*3; ) {
                if (i == nSamples-1) {
                    if (j == 3*nSamples-3) {
                        float currX = vertices[j + (i*nSamples*3)+(n*nSamples*nSamples*3)]; // - 0.001;
                        float prevXS = vertices[j-3 + (i*nSamples*3)+(n*nSamples*nSamples*3)];
                        float prevXT = vertices[j++ + ((i-1)*nSamples*3)+(n*nSamples*nSamples*3)];
    
                        float currY = vertices[j + (i*nSamples*3)+(n*nSamples*nSamples*3)];// + 0.001;
                        float prevYS = vertices[j-3 + (i*nSamples*3)+(n*nSamples*nSamples*3)];
                        float prevYT = vertices[j++ + ((i-1)*nSamples*3)+(n*nSamples*nSamples*3)];
    
                        float currZ = vertices[j + (i*nSamples*3)+(n*nSamples*nSamples*3)]; //+ 0.001;
                        float prevZS = vertices[j-3 + (i*nSamples*3)+(n*nSamples*nSamples*3)];
                        float prevZT = vertices[j++ + ((i-1)*nSamples*3)+(n*nSamples*nSamples*3)];
    
                        glm::vec3 vecS = {prevXS-currX, prevYS-currY, prevZS-currZ};
                        glm::vec3 vecT = {prevXT-currX, prevYT-currY, prevZT-currZ};
                        glm::vec3 normal = glm::normalize(glm::cross(vecS, vecT));
        
                        normals.push_back(normal.x);
                        normals.push_back(normal.y);
                        normals.push_back(normal.z);

                        // std::cout << "X: " << vecS.x << " ";
                        // std::cout << "Y: " << vecS.y << " ";
                        // std::cout << "Z: " << vecS.z << std::endl; 

                        // std::cout << "X: " << vecT.x << " ";
                        // std::cout << "Y: " << vecT.y << " ";
                        // std::cout << "Z: " << vecT.z << std::endl; 

                        // std::cout << "X: " << normal.x << " ";
                        // std::cout << "Y: " << normal.y << " ";
                        // std::cout << "Z: " << normal.z << std::endl << std::endl; 



                        // std::cout << "s-, t-      ";
                        // std::cout << "currX: " << vertices[j + (i*nSamples*3)+(n*nSamples*nSamples*3)] << " ";
                        // std::cout << "prevXS: " << vertices[j - 3 + (i*nSamples*3)+(n*nSamples*nSamples*3)] << " ";
                        // std::cout << "prevXT: " << vertices[j++ + ((i-1)*nSamples*3)+(n*nSamples*nSamples*3)] << std::endl;
                    }
                    else {
                        float currX = vertices[j + (i*nSamples*3)+(n*nSamples*nSamples*3)];
                        float nextXS = vertices[j+3 + (i*nSamples*3)+(n*nSamples*nSamples*3)];
                        float prevXT = vertices[j++ + ((i-1)*nSamples*3)+(n*nSamples*nSamples*3)];
    
                        float currY = vertices[j + (i*nSamples*3)+(n*nSamples*nSamples*3)];
                        float nextYS = vertices[j+3 + (i*nSamples*3)+(n*nSamples*nSamples*3)];
                        float prevYT = vertices[j++ + ((i-1)*nSamples*3)+(n*nSamples*nSamples*3)];
    
                        float currZ = vertices[j + (i*nSamples*3)+(n*nSamples*nSamples*3)];
                        float nextZS = vertices[j+3 + (i*nSamples*3)+(n*nSamples*nSamples*3)];
                        float prevZT = vertices[j++ + ((i-1)*nSamples*3)+(n*nSamples*nSamples*3)];
    
                        glm::vec3 vecS = {nextXS-currX, nextYS-currY, nextZS-currZ};
                        glm::vec3 vecT = {prevXT-currX, prevYT-currY, prevZT-currZ};
                        glm::vec3 normal = glm::normalize(glm::cross(vecT, vecS));
        
                        normals.push_back(normal.x);
                        normals.push_back(normal.y);
                        normals.push_back(normal.z);

                        if (std::isnan(normal.x)) {

                            std::cout << "X: " << currX << " ";
                            std::cout << "Y: " << currY << " ";
                            std::cout << "Z: " << currZ << std::endl; 

                            std::cout << "X: " << vecS.x << " ";
                            std::cout << "Y: " << vecS.y << " ";
                            std::cout << "Z: " << vecS.z << std::endl; 

                            std::cout << "X: " << vecT.x << " ";
                            std::cout << "Y: " << vecT.y << " ";
                            std::cout << "Z: " << vecT.z << std::endl; 

                            std::cout << "X: " << normal.x << " ";
                            std::cout << "Y: " << normal.y << " ";
                            std::cout << "Z: " << normal.z << std::endl << std::endl; 
                        }


                        // std::cout << "s-, t+      ";
                        // std::cout << "currX: " << vertices[j + (i*nSamples*3)+(n*nSamples*nSamples*3)] << " ";
                        // std::cout << "nextXS: " << vertices[j + 3 + (i*nSamples*3)+(n*nSamples*nSamples*3)] << " ";
                        // std::cout << "nextXT: " << vertices[j++ + ((i-1)*nSamples*3)+(n*nSamples*nSamples*3)] << std::endl;
                    }
                }
                else {
                    if (j == 3*nSamples-3) {
                        float currX = vertices[j + (i*nSamples*3)+(n*nSamples*nSamples*3)];
                        float prevXS = vertices[j-3 + (i*nSamples*3)+(n*nSamples*nSamples*3)];
                        float nextXT = vertices[j++ + ((i+1)*nSamples*3)+(n*nSamples*nSamples*3)];
    
                        float currY = vertices[j + (i*nSamples*3)+(n*nSamples*nSamples*3)];
                        float prevYS = vertices[j-3 + (i*nSamples*3)+(n*nSamples*nSamples*3)];
                        float nextYT = vertices[j++ + ((i+1)*nSamples*3)+(n*nSamples*nSamples*3)];
    
                        float currZ = vertices[j + (i*nSamples*3)+(n*nSamples*nSamples*3)];
                        float prevZS = vertices[j-3 + (i*nSamples*3)+(n*nSamples*nSamples*3)];
                        float nextZT = vertices[j++ + ((i+1)*nSamples*3)+(n*nSamples*nSamples*3)];
    
                        glm::vec3 vecS = {prevXS-currX, prevYS-currY, prevZS-currZ};
                        glm::vec3 vecT = {nextXT-currX, nextYT-currY, nextZT-currZ};
                        glm::vec3 normal = glm::normalize(glm::cross(vecT, vecS));
        
                        normals.push_back(normal.x);
                        normals.push_back(normal.y);
                        normals.push_back(normal.z);

                        // std::cout << "s+, t-      ";
                        // std::cout << "currX: " << vertices[j + (i*nSamples*3)+(n*nSamples*nSamples*3)] << " ";
                        // std::cout << "nextXS: " << vertices[j - 3 + (i*nSamples*3)+(n*nSamples*nSamples*3)] << " ";
                        // std::cout << "nextXT: " << vertices[j++ + ((i+1)*nSamples*3)+(n*nSamples*nSamples*3)] << std::endl;
                    }
                    else {
                        float currX = vertices[j + (i*nSamples*3)+(n*nSamples*nSamples*3)];
                        float nextXS = vertices[j + 3 + (i*nSamples*3)+(n*nSamples*nSamples*3)];
                        float nextXT = vertices[j++ + ((i+1)*nSamples*3)+(n*nSamples*nSamples*3)];
        
                        float currY = vertices[j + (i*nSamples*3)+(n*nSamples*nSamples*3)];
                        float nextYS = vertices[j + 3 + (i*nSamples*3)+(n*nSamples*nSamples*3)];
                        float nextYT = vertices[j++ + ((i+1)*nSamples*3)+(n*nSamples*nSamples*3)];
        
                        float currZ = vertices[j + (i*nSamples*3)+(n*nSamples*nSamples*3)];
                        float nextZS = vertices[j + 3 + (i*nSamples*3)+(n*nSamples*nSamples*3)];
                        float nextZT = vertices[j++ + ((i+1)*nSamples*3)+(n*nSamples*nSamples*3)];
                        
                        glm::vec3 vecS = {nextXS-currX, nextYS-currY, nextZS-currZ};
                        glm::vec3 vecT = {nextXT-currX, nextYT-currY, nextZT-currZ};
                        glm::vec3 normal = glm::normalize(glm::cross(vecS, vecT));
        
                        normals.push_back(normal.x);
                        normals.push_back(normal.y);
                        normals.push_back(normal.z);

                        // std::cout << "s+, t+      ";
                        // std::cout << "currX: " << vertices[j + (i*nSamples*3)+(n*nSamples*nSamples*3)] << " ";
                        // std::cout << "nextXS: " << vertices[j + 3 + (i*nSamples*3)+(n*nSamples*nSamples*3)] << " ";
                        // std::cout << "nextXT: " << vertices[j++ + ((i+1)*nSamples*3)+(n*nSamples*nSamples*3)] << std::endl;
                    }

                }
                //j++; j++;

            }
        }
    }*/
    
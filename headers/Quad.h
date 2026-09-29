#ifndef QUAD_H
#define QUAD_H

#include <vector>
#include <string>
#include <GL/glew.h>

class Quad {
    public:
        Quad(const std::string& objPath);
        ~Quad();
    
        void draw();
        void load();
        void initVBO();  // Add this as a member function
    
    private:
        GLuint VAO, VBO, EBO;
        
        std::vector<float> vertices;
        std::vector<float> normals;
        std::vector<float> texCoords;
        std::vector<unsigned int> indices;
        std::vector<unsigned int> normalIndices;
    
        std::string objFile;
    
        void loadOBJ();
    };

#endif

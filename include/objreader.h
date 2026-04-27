#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <iostream>
#include <cmath>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/quaternion.hpp>
#include <glm/glm/gtx/transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>

#include <filesystem>
#include <sstream>
#include <fstream>

// struct to hold the read mesh data from obj file.
struct meshio
{
    // for temporary storage of read data from obj.
    std::vector <float> positiondata;
    std::vector <float> normaldata;
    std::vector <float> bufferdata;   
};

class Mesh
{
    public:
    std::vector <float> data ;
    unsigned int vertexcount;
    unsigned int VBO;
    unsigned int VAO;

    // Function to generate VBO , VAO objects according to the mesh data.
    void genBufferObjects();

    // Mesh Generation Methods
    void gencuboidmesh(glm::vec3 sides);
    void gengridmesh(float interval);
    void genplanemesh(float size);
    void genvectormesh(const float& mag , const glm::vec3& dir , const glm::vec3& position);
    void genbasismesh();
    void genfromobj(std::string path);

};


// class to create mesh objects from obj files.
struct ObjReader
{
    static void ReadMeshFromObjFile(Mesh* object ,std::string path);

    // Mesh Reader helper functions.
    static void ReadVertexData (meshio& iotemp ,std::stringstream &s);
    static void ReadFaceData (meshio& iotemp ,std::stringstream &s);
    static void ReadNormalsData (meshio& iotemp ,std::stringstream &s);

};
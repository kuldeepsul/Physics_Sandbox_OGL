#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <iostream>
#include <cmath>
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>

#include <filesystem>
#include <sstream>
#include <fstream>

struct meshio
{
    std::vector <float> positiondata;
    std::vector <float> normaldata;
    std::vector <float> bufferdata;   
};

struct bound
{
    float negybound ;
    float posybound ;
    float poszbound;
    float negzbound;
    float posxbound;
    float negxbound; 

    bound(float xp , float xn , float yp , float yn , float zp , float zn) :
    posxbound(xp) , negxbound(xn),
    posybound(yp) , negybound(yn),
    poszbound(zp) , negzbound(zn){};

};

class mesh
{
    public:
    std::vector <float> data ;
    unsigned int vertexcount;
    unsigned int VBO;
    unsigned int VAO;

    void genBufferObjects();
};

class MeshGenerator
{
    public:
    static mesh gencuboidmesh(glm::vec3 sides);
    static mesh genplanemesh(float a , float b);
    static mesh readobj(std::string path);

    static void readvertex (meshio& iotemp ,std::stringstream &s);
    static void readface (meshio& iotemp ,std::stringstream &s);
    static void readnormal (meshio& iotemp ,std::stringstream &s);
};


enum shapetype {sphere,cube,plane};

struct rigidbody
{
    // Physics Parameters
    glm::vec3 position = {0.0f , 0.0f ,0.0f};
    glm::vec3 velocity = {0.0f ,0.0f ,0.0f };
    glm::vec3 acceleration = {0.0f ,0.0f ,0.0f };
    float mass {1.0f};

    bool isCollider {true};

    // Shape of body
    shapetype s ;
    
    // Dimesions
    float radius ;                      // for sphere 
    glm::vec3 hcubeside;                // for cube 
    glm::vec2 hplaneside;               // for plane 
    glm::vec3 normplane;                // for plane


    rigidbody(shapetype s_param,float radii);
    rigidbody(shapetype s_param,glm::vec3 side);
    rigidbody(shapetype s_param,glm::vec2 sides , glm::vec3 norm);

    // Collisions
    void checkboundcollision(bound &domain);
    void checkAABB(rigidbody* other);
};

class Entity
{
    public:
    // Unique Id 
    const unsigned int id;
    std::string name;

    // Rendering Parameters
    glm::vec3 col;
    glm::mat4 model_matrix = glm::mat4 (1.0f);
    bool isWireFrame {false};

    // Attachments 
    mesh* entitymesh;
    rigidbody* entitybody;



    Entity() = delete ;
    Entity(unsigned int i) : id(i) {} ;

    void translateEntity(glm::vec3 disp);
    void rotateEntity(float angle, glm::vec3 axis);
    void scaleEntity(glm::vec3 axis);

    void updateModelMatrix();

};

class Scene
{
    public:
    std::vector <Entity*> entities;

    void newEntity(unsigned int id , shapetype s , glm::vec3 sides);

};






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

    void gencuboidmesh(glm::vec3 sides);
    void readobj(std::string path);

    void readvertex (meshio& iotemp ,std::stringstream &s);
    void readface (meshio& iotemp ,std::stringstream &s);
    void readnormal (meshio& iotemp ,std::stringstream &s);
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

    // Orientation
    glm::quat orientation = {1.0f,0.0f,0.0f,0.0f};

    // Shape of body
    shapetype s ;
    
    // Dimesions
    glm::vec3 hcubeside;                // for cube 

    
    rigidbody(shapetype s_param,glm::vec3 side);

    void updateorientation(float angle,glm::vec3 axisofrotation);


    // Collision Utils
    std::vector <glm::vec3> getvertexdata();
    std::vector <glm::vec3> getSATaxes(const std::vector <glm::vec3> &vertdataA , const std::vector <glm::vec3> &vertdataB) const;
    glm::vec2 getMinMaxprojection(const glm::vec3 &axis, const std::vector <glm::vec3> & vertexdata) const;
    float getaxispenetration(const glm::vec3& axis,const std::vector <glm::vec3>& vertdataA , const std::vector<glm::vec3>& vertdataB) const;

    // Collisions
    void checkboundcollision(rigidbody* domain);
    void checkAABB(rigidbody* other);
    bool checkSAT(rigidbody* other);
    
};

class Entity
{
    public:
    // Unique Id 
    unsigned int id {0};
    std::string name =  {"no_name"};

    // Rendering Parameters
    glm::vec3 col {1.0f,1.0f,1.0f};
    glm::mat4 model_matrix = glm::mat4 (1.0f);
    bool isWireFrame {false};

    // Attachments 
    mesh* entitymesh = nullptr;
    rigidbody* entitybody = nullptr;



    Entity();
    Entity(unsigned int i) : id(i) {} ;
    ~Entity()
    {
        if ( entitymesh)
        {
            delete entitymesh;
        }
        
        if (entitybody)
        {
            delete entitybody;
        }
    }


    void updateModelMatrix();

};

class Scene
{
    public:
    std::vector <Entity*> entities;
    Entity* scene_bound;

    Entity* newEntity(unsigned int id);
    void newEntity(unsigned int id , shapetype s , glm::vec3 sides);
    

};






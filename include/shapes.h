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

std::string readShaderFile(std::string path);
void checkShaderCompilation(unsigned int &shader);
unsigned int getShaderProgram(std::string& path_vert,std::string path_frag);

struct meshio
{
    std::vector <float> positiondata;
    std::vector <float> normaldata;
    std::vector <float> bufferdata;   
};

class GeomFunc
{
    public:
    static float ClosestPtSegementSegment                       // Return Distance squared between the closest point on segement.
    (
    const glm::vec3 &p1 , const glm::vec3 &q1 ,
    const glm::vec3 &p2 , const glm::vec3 &q2,
    glm::vec3 &c1 , glm::vec3 &c2
    );
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
    void gengridmesh(float interval);
    void genplanemesh(float size,glm::vec3 center,glm::vec3 normal);
    void genvectormesh(const float& mag , const glm::vec3& dir , const glm::vec3& position);
    void readobj(std::string path);

    void readvertex (meshio& iotemp ,std::stringstream &s);
    void readface (meshio& iotemp ,std::stringstream &s);
    void readnormal (meshio& iotemp ,std::stringstream &s);
};

struct contact
{
    
    glm::vec3 normal;
    glm::vec3 point;
    float depth;

};

enum shapetype {sphere,cube,plane};

class Scene;
struct rigidbody
{
    bool isCollider {true};

    // Physics Parameters
    glm::vec3 position = {0.0f , 0.0f ,0.0f};
    glm::quat orientation = {1.0f,0.0f,0.0f,0.0f};
    glm::vec3 lmomentum = {0.0f,0.0f,0.0f};
    glm::vec3 amomentum = {0.0f,0.0f,0.0f};
    
    // Derived 
    glm::vec3 velocity = {0.0f ,0.0f ,0.0f };
    glm::vec3 acceleration = {0.0f ,0.0f ,0.0f };

    // Constants
    float mass {1.0f};

    // Contact points
    rigidbody* contactbody;
    contact* bodycontact;

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
    bool checkinrange(const float& val ,const float& range1,const float& range2);

    // Collisions
    void checkboundcollision(rigidbody* domain);
    void checkAABB(rigidbody* other);
    bool checkSAT(rigidbody* other);

    // Contact Generation.
    void createcontactbodies(Scene* s2);
    void updatecontact();
    void getcontactpoints(rigidbody* other);
    std::vector <bool> checkvertexinclusion(const std::vector <glm::vec3> &cubedataA , const std::vector <glm::vec3> &cubedata);
    
    
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
    std::string vertex_shader;
    std::string fragment_shader;
    unsigned int shaderprogram;

    Entity* newEntity(unsigned int id);
    Entity* newEntity(unsigned int id , shapetype s , glm::vec3 sides);

    // Utils.
    void showgrids(float intervals);
    void drawcontactnormal(rigidbody* body);
    void SATAxes();
    

};






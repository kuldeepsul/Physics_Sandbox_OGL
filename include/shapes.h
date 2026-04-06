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
    // for temporary storage of read data from obj.
    std::vector <float> positiondata;
    std::vector <float> normaldata;
    std::vector <float> bufferdata;   
};

struct GeomFunc
{
    static float ClosestPtSegementSegment                       // Return Distance squared between the closest point on segement.
    (
    const glm::vec3 &p1 , const glm::vec3 &q1 ,
    const glm::vec3 &p2 , const glm::vec3 &q2,
    glm::vec3 &c1 , glm::vec3 &c2
    );

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
    void genplanemesh(float size,glm::vec3 center,glm::vec3 normal);
    void genvectormesh(const float& mag , const glm::vec3& dir , const glm::vec3& position);
    void genfromobj(std::string path);


};

struct ObjReader
{
    static void ReadMeshFromObjFile(Mesh* object ,std::string path);

    // Mesh Reader helper functions.
    static void ReadVertexData (meshio& iotemp ,std::stringstream &s);
    static void ReadFaceData (meshio& iotemp ,std::stringstream &s);
    static void ReadNormalsData (meshio& iotemp ,std::stringstream &s);

};


struct contact
{
    
    glm::vec3 normal;
    glm::vec3 point;
    float depth;

};

enum class ShapeType {sphere,cube,plane};

struct RigidBody
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

    // Shape of body
    ShapeType s ;
    glm::vec3 hcubeside;  

    // Contacts
    std::vector <contact> cons;

    
    RigidBody(ShapeType s_param,glm::vec3 side);
    void updateorientation(float angle,glm::vec3 axisofrotation);

    // Collision Resolution.
    void resolvecontacts();

};

struct CollisionFunc
{
    // Collision Utils
    static std::vector <glm::vec3> getvertexdata(RigidBody* body);
    static std::vector <glm::vec3> getSATaxes(const std::vector <glm::vec3> &vertdataA , const std::vector <glm::vec3> &vertdataB);
    static glm::vec2 getMinMaxprojection(const glm::vec3 &axis, const std::vector <glm::vec3> & vertexdata);
    static bool checkinrange(const float& val ,const float& range1,const float& range2);

    static glm::vec3 getcontactpoint(const std::vector <glm::vec3> &vertdataA , 
                                    const std::vector <glm::vec3> &vertdataB ,
                                    const glm::vec3 &axis , 
                                    const int &axis_id
    );

    // Return pair of edges , represented by the pair of ids forming these edges.
    static std::pair <std::pair<int,int>,std::pair<int,int>> getcollisionedges(const std::vector <glm::vec3> &vertdataA , 
                                                                                const std::vector <glm::vec3> &vertdataB , 
                                                                                const glm::vec3 &axis , 
                                                                                const int &axis_id
    );

    // Collisions
    static int checkvertexinclusion(const std::vector <glm::vec3> &cubedataA , const std::vector <glm::vec3> &cubedataB, const int &axis_id , const glm::vec3 &axis );
    static void checkboundcollision(RigidBody* body ,RigidBody* domain);
    static void checkAABB(RigidBody* body1 , RigidBody* body2);
    static bool checkSAT(RigidBody* body1 , RigidBody* body2);
    

    
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
    Mesh* entitymesh = nullptr;
    RigidBody* entitybody = nullptr;



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
    Entity* newEntity(unsigned int id , ShapeType s , glm::vec3 sides);

    void drawScene(glm::mat4 &persp , glm::mat4 &view , glm::vec3 lightpos);

    // Utils.
    void showgrids(float intervals);
    void showcontacts(RigidBody* body);

};

class DebugScene
{
    std::vector <Entity*> entities;

};






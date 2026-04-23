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
    void genplanemesh(float size);
    void genvectormesh(const float& mag , const glm::vec3& dir , const glm::vec3& position);
    void genbasismesh();
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

struct RigidBody;

struct contact
{
    
    glm::vec3 normal;
    glm::vec3 point;
    float depth;

    RigidBody* bodyA;
    RigidBody* bodyB;
};

enum class ShapeType {cube,plane};

struct RigidBody
{
    bool isCollider {true};
    bool isStatic {false};

    // Physics Parameters
    glm::vec3 position = {0.0f , 0.0f ,0.0f};
    glm::quat orientation = {1.0f,0.0f,0.0f,0.0f};
    glm::vec3 lmomentum = {0.0f,0.0f,0.0f};
    glm::vec3 amomentum = {0.0f,0.0f,0.0f};
    
    // Derived 
    glm::vec3 velocity = {0.0f ,0.0f ,0.0f };
    glm::vec3 avelocity = {0.0f,0.0f,0.0f};

    // Constants
    float mass {1.0f};
    float invmass {1.0f};
    glm::mat3 InertiaG {1.0f};
    glm::mat3 invInertiaG {1.0f};
    float restitution {0.3};

    // Local Moment of Inertia.
    glm::mat3 InertiaL {1.0f};
    glm::mat3 invInertiaL{1.0f};

    // Force and Torque.
    glm::vec3 Force {0.0f,0.0f,0.0f};   
    glm::vec3 Torque {0.0f,0.0f,0.0f};

    // Shape of body
    ShapeType s = ShapeType::cube ;
    glm::vec3 hcubeside;  
    glm::vec3 planenormal {0.0f,1.0f,0.0f};
    
    RigidBody(ShapeType s_param,glm::vec3 side);
    void updateorientation(float angle,glm::vec3 axisofrotation);
    void genLocalInertiaMatrix();
    void updatestate(const float &dt);
};

struct CollisionFunc
{
    // Collision Utils
    static std::vector <glm::vec3> getvertexdata(RigidBody* body);
    static std::vector <glm::vec3> getSATaxes(const RigidBody* bodyA ,const RigidBody* bodyB);
    static glm::vec2 getMinMaxprojection(const glm::vec3 &axis, const std::vector <glm::vec3> & vertexdata);
    static bool checkinrange(const float& val ,const float& range1,const float& range2);

    static glm::vec3 getcontactpoint(RigidBody* BodyA , RigidBody* BodyB ,
                                    const glm::vec3 &axis , 
                                    const int &axis_id
    );

    static glm::vec3 getedgeedgecontactpoint( RigidBody* BodyA , RigidBody* BodyB , 
                                        const glm::vec3 &axis , 
                                        const int &axis_id
    );

    // Collisions
    static glm::vec3 getvertexfacecontactpoint( RigidBody* BodyB , const glm::vec3 &axis );
    static void checkboundcollision(RigidBody* body ,RigidBody* domain);
    static void checkAABB(RigidBody* body1 , RigidBody* body2);
    static bool checkSAT(RigidBody* body1 , RigidBody* body2,std::vector <contact*> &condata,std::vector <glm::vec3> &checkaxes);
    static bool checkPlaneBox(RigidBody* body1,RigidBody* body2,std::vector <contact*> &condata);
    
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
    std::vector <contact*> contacts;
    std::vector <contact*> groundcontacts;
    std::vector <Mesh*> debugvectors;
    std::vector <glm::vec3> SATaxes;
    Entity* scene_bound;
    unsigned int shaderprogram;
    Mesh* contactmesh = nullptr;


    Entity* newEntity(unsigned int id);
    Entity* newEntity(unsigned int id , ShapeType s , glm::vec3 sides);

    void stepphysics(const float &dt);
    void gencontactdata();
    void resolvegroundcontacts();
    void refreshDebugData();
    void resolveContacts();

    void drawScene(glm::mat4 &persp , glm::mat4 &view , glm::vec3 lightpos,int gl_primitive);
    

    // Utils.
    void showgrids(float intervals);
    void drawcontacts(glm::mat4 &persp , glm::mat4 &view , glm::vec3 lightpos,int gl_primitive);
    void drawobjectbasisvectors(glm::mat4 &persp , glm::mat4 &view , glm::vec3 lightpos,int gl_primitive);

};

struct PhysicsFunc
{

};








#pragma once
#include "objreader.h"
#include "collision.h"


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
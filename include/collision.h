#pragma once
#include "objreader.h"

struct RigidBody;
struct contact;

struct GeomFunc
{
    static float ClosestPtSegementSegment                       // Return Distance squared between the closest point on segement.
    (
    const glm::vec3 &p1 , const glm::vec3 &q1 ,
    const glm::vec3 &p2 , const glm::vec3 &q2,
    glm::vec3 &c1 , glm::vec3 &c2
    );

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
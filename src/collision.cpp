#include "collision.h"
#include "shapes.h"

///////////////////////////////////////////////////////////
//////          Geometry Helper function        //////////

float GeomFunc::ClosestPtSegementSegment                       // Return Distance squared between the closest point on segement.
(
const glm::vec3 &p1 , const glm::vec3 &q1 ,
const glm::vec3 &p2 , const glm::vec3 &q2,
glm::vec3 &c1 , glm::vec3 &c2
)
{
    // Calculating required quantities.
    glm::vec3 d1 = q1 - p1 ;
    glm::vec3 d2 = q2 - p2 ;
    glm::vec3 r  = p1 - p2 ;

    float a  = glm::dot (d1,d1);
    float b  = glm::dot (d1,d2);
    float c  = glm::dot (d1,r);
    float f  = glm::dot (d2,r);
    float e  = glm::dot (d2,d2);

    float s , t ;

    // Checking cases if segments S1 & S2 both are of zero length
    if (a <= FLT_EPSILON && e <= FLT_EPSILON)
    {
        s = t = 0.0f ;
        c1 = p1 ;
        c2 = p2 ;
        return glm::dot((c2 - c1) , (c2 - c1));
    }

    if( a <= FLT_EPSILON )
    {
        s = 0.0f ;
        t = f / e ;
        t = glm::clamp( t , 0.0f , 1.0f );
    }
    else
    {
        if( e <= FLT_EPSILON)
        {
            t = 0.0f ;
            s = -c / a ;
            s = glm::clamp( s , 0.0f , 1.0f);            
        }
        else
        {
            // General case when both segments of non zero length.
            // First we find the closest point on lines L1 and L2 , and check if they lie on segment.
            float denom = (a * e) - (b * b);
            if( denom != 0.0f )
            {
                s = ((b * f) - (e * c))/denom;
                s = glm::clamp( s , 0.0f , 1.0f);
            }
            else
            {
                // If the lines are parallel , we pick an arbitrary point on segment S1 , and find point on S2 closest to it .
                // In this case we pick s = 0.0f , that is the lowermost end of the segment S1.
                s = 0.0f;
            }
        }
    }
            
    t =( (b * s) + f) / e ;

    // If t lies [0,1] then we are done , if not then we clamp t , and recompute s , that is the closest point to current t .

    if( t < 0.0f)
    {
        t = 0.0f ;
        s = (-c / a);
        s = glm::clamp( s , 0.0f ,1.0f);
    }
    else if ( t > 1.0f )
    {
        t = 1.0f ;
        s = ((b - c) / a);
        s = glm::clamp( s , 0.0f ,1.0f);
    }

    c1 = p1 + s * d1 ;
    c2 = p2 + t * d2 ;
    return glm::dot((c2 - c1),(c2 - c1));
        
    

}

//////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////
//////          Collision Detection                     /////////

void CollisionFunc::checkboundcollision(RigidBody* body ,RigidBody* domain)
{
    glm::vec3 HEbody = body->position + body->hcubeside;
    glm::vec3 LEbody = body->position - body->hcubeside;

    glm::vec3 HEbound = domain->position + domain->hcubeside;
    glm::vec3 LEbound = domain->position - domain->hcubeside;

    // Lower bounds .
    if (LEbody.y < LEbound.y)
    {
        body->position.y = LEbound.y + body->hcubeside.y ;
        body->velocity.y = -body->velocity.y ;
    }
    // Upper bounds.
    else if (HEbody.y > HEbound.y)
    {
        body->position.y = HEbound.y - body->hcubeside.y ;
        body->velocity.y = -body->velocity.y ;
    }

    // Left bounds .
    if (LEbody.x < LEbound.x)
    {
        body->position.x = LEbound.x + body->hcubeside.x ;
        body->velocity.x = -body->velocity.x ;
        
    }
    // Right bounds.
    else if (HEbody.x > HEbound.x)
    {
        body->position.x =HEbound.x - body->hcubeside.x ;
        body->velocity.x = -body->velocity.x ;      
    }

    // Front bounds .
    if (LEbody.z < LEbound.z)
    {
        body->position.z = LEbound.z + body->hcubeside.z ;
        body->velocity.z = -body->velocity.z ;
    }
    // Rear bounds.
    else if (HEbody.z > HEbound.z)
    {
        body->position.z = HEbound.z - body->hcubeside.z ;
        body->velocity.z = -body->velocity.z ;
    }
};

void CollisionFunc::checkAABB(RigidBody* body1 , RigidBody* body2)
{
    // check overlap
    float penx = 0.0f ;
    float peny = 0.0f ;
    float penz = 0.0f ; 

    bool overlapx {false};
    bool overlapy {false};
    bool overlapz {false};

    // in X 

    if (body1->position.x >  body2->position.x)
    {
        penx = (body2->position.x + body2->hcubeside.x) - (body1->position.x - body1->hcubeside.x);

        if (penx > 0)
        {
            //other->position.x  -= 0.5*penx;
            //this->position.x += 0.5*penx;
            overlapx = true ;
        }
    }
    else
    {
        penx = (body1->position.x + body1->hcubeside.x) - (body2->position.x - body2->hcubeside.x) ;

        if (penx > 0)
        {
            //this->position.x -= 0.5 * penx; 
            //other->position.x += 0.5 * penx;          
            overlapx = true ;
        }
    }

    // in Y
    if (body1->position.y >  body2->position.y)
    {
        peny = (body2->position.y + body2->hcubeside.y) - (body1->position.y - body1->hcubeside.y);

        if (peny > 0)
        {
            // other->position.y  -= 0.5*peny;
            // this->position.y += 0.5*peny;
            overlapy = true ;
        }
    }
    else
    {
        peny = (body1->position.y + body1->hcubeside.y) - (body2->position.y - body2->hcubeside.y) ;

        if (peny > 0)
        {
            // this->position.y -= 0.5 * peny; 
            // other->position.y += 0.5 * peny; 
            overlapy = true ;
        }
    }

    // in Z
    if (body1->position.z >  body2->position.z)
    {
        penz = (body2->position.z + body2->hcubeside.z) - (body1->position.z - body1->hcubeside.z);

        if (penz > 0)
        {
            // other->position.z  -= 0.5*penz;
            // this->position.z += 0.5*penz;
            overlapz = true ;
        }
    }
    else
    {
        penz = (body1->position.z + body1->hcubeside.z) - (body2->position.z - body2->hcubeside.z) ;

        if (penz > 0)
        {
            // this->position.z -= 0.5 * penz; 
            // other->position.z += 0.5 * penz; 
            overlapz = true ;
        }
    }

    if (overlapx && overlapy && overlapz)
    {
        float m1 = body1->mass;
        float m2 = body2->mass;

        // Collision resolution. 

        if (penx < peny && penx < penz)
        {
            float vi1 = body1->velocity.x;
            float vi2 = body2->velocity.x;

            body1->velocity.x = ((m1-m2)/(m1+m2))*vi1 + ((2*m2)/(m1 + m2))* vi2;
            body2->velocity.x =((m2-m1)/(m1+m2))*vi2 + ((2*m1)/(m1 + m2))* vi1;
        }
        else if (peny < penx && peny < penz)
        {
            float vi1 = body1->velocity.y;
            float vi2 = body2->velocity.y;

            body1->velocity.y = ((m1-m2)/(m1+m2))*vi1 + ((2*m2)/(m1 + m2))* vi2;
            body2->velocity.y =((m2-m1)/(m1+m2))*vi2 + ((2*m1)/(m1 + m2))* vi1;
        }
        else if (penz < penx && penz < peny)
        {
            float vi1 = body1->velocity.z;
            float vi2 = body2->velocity.z;

            body1->velocity.z = ((m1-m2)/(m1+m2))*vi1 + ((2*m2)/(m1 + m2))* vi2;
            body2->velocity.z =((m2-m1)/(m1+m2))*vi2 + ((2*m1)/(m1 + m2))* vi1;
        }
    }
};

bool CollisionFunc::checkinrange(const float& val ,const float& range1,const float& range2)
{
    float dif1 = val - range1;
    float dif2 = val - range2;

    if (dif1 == 0 || dif2 == 0)
    {
        return false ;
    }
    if( dif1 > 0 && dif2 > 0 || dif1 < 0 && dif2 < 0)
    {
        return false;
    }
    else 
    {
        return true;
    }

}

glm::vec3 CollisionFunc::getvertexfacecontactpoint( RigidBody* BodyB , const glm::vec3 &axis )
{
    glm::vec3 centerB = BodyB->position;
    glm::vec3 extentsB = BodyB->hcubeside;
    glm::mat3 modelB = glm::mat3_cast(BodyB->orientation);

    glm::vec3 iB = glm::normalize(modelB[0]);
    glm::vec3 jB = glm::normalize(modelB[1]);
    glm::vec3 kB = glm::normalize(modelB[2]);

    float signiB = (glm::dot(axis,iB) > 0) ? 1.0f : -1.0f;
    float signjB = (glm::dot(axis,jB) > 0) ? 1.0f : -1.0f;
    float signkB = (glm::dot(axis,kB) > 0) ? 1.0f : -1.0f;


    glm::vec3 vertex = centerB + signiB * modelB[0] * extentsB[0] + signjB * modelB[1] * extentsB[1] + signkB * modelB[2] * extentsB[2];

    return vertex;
}

glm::vec3 CollisionFunc::getedgeedgecontactpoint(RigidBody* BodyA , RigidBody* BodyB , 
                            const glm::vec3 &axis , 
                            const int &axis_id
)
{
    glm::vec3 centerA = {BodyA->position.x ,BodyA->position.y ,BodyA->position.z };
    glm::vec3 extentsA = BodyA->hcubeside;
    glm::mat3 modelA = glm::mat3_cast(BodyA->orientation);


    glm::vec3 centerB = BodyB->position;
    glm::vec3 extentsB = BodyB->hcubeside;
    glm::mat3 modelB = glm::mat3_cast(BodyB->orientation);

    // Based on Axis we need to shortlist the edges direction.
    int iA = std::floor(axis_id / 3);
    iA = iA - 2; 
    glm::vec3 axisA = modelA[iA];

    int iB = axis_id % 3;
    glm::vec3 axisB = modelB[iB];

    int jA = (iA + 1) % 3;
    int kA = (iA + 2) % 3;

    int jB = (iB + 1) % 3;
    int kB = (iB + 2) % 3;

    float signjA = (glm::dot(axis,modelA[jA]) > 0) ? 1.0f : -1.0f;
    float signkA = (glm::dot(axis,modelA[kA]) > 0) ? 1.0f : -1.0f;

    float signjB = (glm::dot(-axis,modelB[jB]) > 0) ? 1.0f : -1.0f;
    float signkB = (glm::dot(-axis,modelB[kB]) > 0) ? 1.0f : -1.0f;

    glm::vec3 edgeAcenter  = centerA + signjA * modelA[jA] * extentsA[jA] + signkA * modelA[kA] * extentsA[kA] ;
    glm::vec3 edgeBcenter  = centerB + signjB * modelB[jB] * extentsB[jB] + signkB * modelB[kB] * extentsB[kB] ;

    glm::vec3 edgeAlow = edgeAcenter - modelA[iA] * extentsA[iA];
    glm::vec3 edgeAhigh = edgeAcenter + modelA[iA] * extentsA[iA];

    glm::vec3 edgeBlow = edgeBcenter - modelB[iB] * extentsB[iB];
    glm::vec3 edgeBhigh = edgeBcenter + modelB[iB] * extentsA[iB];

    glm::vec3 conA , conB;

    float d2 = GeomFunc::ClosestPtSegementSegment(edgeAlow,edgeAhigh,edgeBlow,edgeBhigh,conA,conB);
    glm::vec3 condir = conB - conA ;

    glm::vec3 result =  conB;

    return result;

}


glm::vec3 CollisionFunc::getcontactpoint(RigidBody* BodyA , RigidBody* BodyB , const glm::vec3 &axis , const int &axis_id)
{
    glm::vec3 collision_point;

    
    if (axis_id < 6)
    {
        // its a face to vertex collision.
        if(axis_id < 3)
        {
            // vertex of second cube is penetrating into first cube.
            collision_point = getvertexfacecontactpoint(BodyB,-axis);
            return collision_point;
        }
        else
        {
            // vertex of first cube is penetrating into second cube.
            collision_point = getvertexfacecontactpoint(BodyA,axis);
            return collision_point;
        }
        
    }
    else
    {
        // its a edge to edge collision.
        std::cout << "Edge - Edge collision." << std::endl;
        collision_point = getedgeedgecontactpoint(BodyA,BodyB,axis,axis_id);
        return collision_point;

    }
}

std::vector <glm::vec3> CollisionFunc::getvertexdata(RigidBody* body)
{
    glm::vec3 hside = body->hcubeside;

    std::vector <glm::vec3> vertexdata;

    // Right Face
    glm::vec3 vert1 = {hside.x, -hside.y,hside.z};
    glm::vec3 vert2 = {hside.x,hside.y,hside.z};
    glm::vec3 vert3 = {hside.x,hside.y,-hside.z};
    glm::vec3 vert4 = {hside.x, -hside.y, -hside.z};

    // Left Face
    glm::vec3 vert5 = { -hside.x, -hside.y,hside.z};
    glm::vec3 vert6 = { -hside.x,hside.y,hside.z};
    glm::vec3 vert7 = { -hside.x,hside.y, -hside.z};
    glm::vec3 vert8 = { -hside.x, -hside.y, -hside.z};


    glm::mat4 rotation_matrix = glm::mat4_cast(body->orientation);

    vertexdata.push_back(glm::mat3(rotation_matrix)*vert1 + body->position);
    vertexdata.push_back(glm::mat3(rotation_matrix)*vert2 + body->position);
    vertexdata.push_back(glm::mat3(rotation_matrix)*vert3 + body->position);
    vertexdata.push_back(glm::mat3(rotation_matrix)*vert4 + body->position);
    vertexdata.push_back(glm::mat3(rotation_matrix)*vert5 + body->position);
    vertexdata.push_back(glm::mat3(rotation_matrix)*vert6 + body->position);
    vertexdata.push_back(glm::mat3(rotation_matrix)*vert7 + body->position);
    vertexdata.push_back(glm::mat3(rotation_matrix)*vert8 + body->position);

    return vertexdata;

}

std::vector <glm::vec3> CollisionFunc::getSATaxes(const RigidBody* bodyA ,const RigidBody* bodyB) 
{
    glm::mat3 modelA = glm::mat3_cast(bodyA->orientation);
    glm::mat3 modelB = glm::mat3_cast(bodyB->orientation);

    glm::vec3 normA1 =  glm::normalize(modelA[0]);
    glm::vec3 normA2 =  glm::normalize(modelA[1]);
    glm::vec3 normA3 =  glm::normalize(modelA[2]);

    
    glm::vec3 normB1 =  glm::normalize(modelB[0]);
    glm::vec3 normB2 =  glm::normalize(modelB[1]);
    glm::vec3 normB3 =  glm::normalize(modelB[2]);

    // Side Normals of first cube.
    normA1 = glm::normalize(normA1);
    normA2 = glm::normalize(normA2);
    normA3 = glm::normalize(normA3);
    // Side Normals of second cube.
    normB1 = glm::normalize(normB1);
    normB2 = glm::normalize(normB2);
    normB3 = glm::normalize(normB3);

    std::vector <glm::vec3> norm_cubeA  = {normA1,normA2,normA3};
    std::vector <glm::vec3> norm_cubeB  = {normB1,normB2,normB3};

    std::vector <glm::vec3> SATaxes = {
    // First Cube
    normA1, normA2, normA3,     
    // Secong Cube
    normB1, normB2, normB3,
    };

    // Cross Normals.
    for (auto& vecA : norm_cubeA)
    {
        for (auto vecB : norm_cubeB)
        {
            glm::vec3 temp = glm::cross(vecA,vecB);
            if(glm::length(temp) < 1.0e-06 )
            {
                continue;
            }
            else
            {
                SATaxes.push_back(glm::normalize(temp));
            }
            
        }
    }
    //std::cout << "SAT axed formed : " << SATaxes.size() << std::endl;
    return SATaxes;

}

glm::vec2 CollisionFunc::getMinMaxprojection(const glm::vec3 &axis, const std::vector <glm::vec3> & vertexdata)
{
    float min = 0;
    float max = 0; 
    bool first_check = true;

    for (const auto& vert : vertexdata)
    {
        float proj = glm::dot(vert,axis); 
        if (first_check)
        {
            min = proj;
            max = proj;
            first_check = false;
        }

        min  = std::min (min,proj);
        max = std::max (max,proj);
    }

    return glm::vec2(min,max);
}

bool CollisionFunc::checkSAT(RigidBody* body1 , RigidBody* body2,std::vector <contact*> &condata,std::vector <glm::vec3> &checkaxes)
{
    std::vector <glm::vec3> vertdataA ;
    std::vector <glm::vec3> vertdataB ;

    vertdataA = getvertexdata(body1);
    vertdataB = getvertexdata(body2);
    

    // Create Normal of faces for SAT Checks.
    
    std::vector <glm::vec3> SATaxes = getSATaxes(body1,body2);
    bool collision = true ;
    float min_collision = 0;
    glm::vec3 collisionnormal;
    int axisid = 0 ;
    bool first_check = true;
    
    for (int i {0} ; i < SATaxes.size() ; i++)
    {
        // storing SAT axes for debugging.
        checkaxes.push_back(SATaxes[i]);
        glm::vec2 projA = getMinMaxprojection(SATaxes[i],vertdataA);
        glm::vec2 projB = getMinMaxprojection(SATaxes[i],vertdataB);

        //std::cout << "Complete!" << std::endl;

        float penetration = 0 ;
        if (projA.x > projB.y || projB.x > projA.y)
        {
            return false;
        }

        penetration = std::min (projA.y,projB.y) - std::max (projB.x ,projA.x);

        if (first_check)
        {
            min_collision = penetration;
            collisionnormal = SATaxes[i];
            axisid = i ;
            first_check = false;
        }
        
        if (penetration < min_collision)
        {
            min_collision = penetration;
            collisionnormal = SATaxes[i];
            axisid = i ;
        }
        
    }

    glm::vec3 dir = body2->position - body1->position;
    if(glm::dot(dir,collisionnormal) < 0)
    {
        collisionnormal = -collisionnormal;
    }

    if(collision)
    {
        std::cout << "Collision axis ID : " << axisid << std::endl;
        std::cout << "Collision Normal : " << "("<< collisionnormal.x << ","
        << collisionnormal.y << "," << collisionnormal.z << ")" << std::endl;
    }


    glm::vec3 conpoint = CollisionFunc::getcontactpoint(body1,body2,collisionnormal,axisid);

    // generating contact.
    contact* con = new contact();
    con->depth = min_collision;
    con->normal = collisionnormal;
    con->point = conpoint;
    con->bodyA = body1;
    con->bodyB = body2;

    // storing contact into contact database.
    condata.push_back(con);
    
    return collision;

}

bool CollisionFunc::checkPlaneBox(RigidBody* body1,RigidBody* body2,std::vector <contact*> &condata)
{
    glm::vec3 colnorm;
    RigidBody* cube;
    RigidBody* plane;

    if(body1->s == ShapeType::plane)
    {
        // check collision taking body1 as plane.
        plane = body1;
        cube = body2;
    }
    else
    {
        // check collision taking body2 as plane.
        plane = body2;
        cube = body1;
    }
    if (glm::dot(cube->position,plane->planenormal) > 0.0f)
    {
        colnorm = -plane->planenormal;
    }
    else if (glm::dot(cube->position,plane->planenormal) < 0.0f)
    {
        colnorm = plane->planenormal;
    }


    glm::vec3 colpoint = CollisionFunc::getvertexfacecontactpoint(cube,colnorm);
    bool collision = false;
    colnorm = plane->planenormal;

    if (glm::dot(colpoint,colnorm) < 0.0f)
    {
        // Collision detected.
        contact* con = new contact();
        collision = true;
        con->depth = std::abs(glm::dot(colpoint,colnorm));
        con->normal = colnorm;
        con->point = colpoint;
        con->bodyA = plane;
        con->bodyB = cube;
        condata.push_back(con);
    }

    return collision;
};


/////////////////////////////////////////////////////////////////////
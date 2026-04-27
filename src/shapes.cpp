#include "shapes.h"
#define PI 3.14159265358979323846



Entity* Scene::newEntity(unsigned int id)
{
    Entity* ent = new Entity(id);
    this->entities.push_back(ent);

    return ent;
}

Entity* Scene::newEntity(unsigned int id , ShapeType s , glm::vec3 sides)
{
    Entity* ent = new Entity(id);

    // Generete Mesh and attach to entity
    Mesh* m = new Mesh(); 
    
    m->gencuboidmesh(sides);
    ent->entitymesh = m;

    // Generete rigid body and attach to entity.
    ent->entitybody = new RigidBody(ShapeType::cube,sides);
    
    this->entities.push_back(ent);
    return ent;
};

void Scene::drawScene(glm::mat4 &persp , glm::mat4 &view , glm::vec3 lightpos,int gl_primitive)
{
    glUseProgram(this->shaderprogram);
    // Perspective 
    unsigned int persloc = glGetUniformLocation(this->shaderprogram,"Perspective_mat");
    glUniformMatrix4fv(persloc,1,GL_FALSE,glm::value_ptr(persp));

    // View Matrix Based on  Camera Controls
    unsigned int viewloc = glGetUniformLocation(this->shaderprogram,"View_mat");
    glUniformMatrix4fv(viewloc,1,GL_FALSE,glm::value_ptr(view));

    // Camera as lightsource
    unsigned int lightposloc = glGetUniformLocation(this->shaderprogram,"lightpos");
    glUniform3fv(lightposloc,1,glm::value_ptr(lightpos));

    for (Entity* ent : this->entities)
    {


        // Passing Entity color as a uniform.
        unsigned int objcolloc = glGetUniformLocation(this->shaderprogram,"objcol");
        glUniform3fv(objcolloc,1,glm::value_ptr(glm::normalize(ent->col)));

        // Passing Entity Model Matrix as a uniform. 
        unsigned int modelloc = glGetUniformLocation(this->shaderprogram,"Model_mat");
        glUniformMatrix4fv(modelloc,1,GL_FALSE,glm::value_ptr(ent->model_matrix));

        if (ent->isWireFrame)
        {
            glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
            glBindVertexArray(ent->entitymesh->VAO);
            glDrawArrays(gl_primitive,0,ent->entitymesh->vertexcount);
        }
        else
        {
            glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
            glBindVertexArray(ent->entitymesh->VAO);
            glDrawArrays(gl_primitive,0,ent->entitymesh->vertexcount);
        }

    }
};

void Entity::updateModelMatrix()
{
    glm::mat4 translation_matrix = glm::translate(glm::mat4 (1.0f) ,this->entitybody->position);

    glm::mat4 rotation_matrix = glm::mat4_cast(this->entitybody->orientation);
    this->model_matrix = translation_matrix * rotation_matrix ;
};


RigidBody::RigidBody(ShapeType s_param,glm::vec3 side)
{
    if(s_param == ShapeType::cube)
    {
        this->s = s_param;
        this->hcubeside = 0.5f * side;
        this->genLocalInertiaMatrix();
    }
    else if (s_param == ShapeType::plane)
    {
        this->s = s_param;
        this->planenormal = {0.0f,1.0f,0.0f};
        //this->invInertiaG = glm::mat3 {0.0f};
        this->invInertiaL = glm::mat3 {0.0f};
        this->invmass = 0.0f;
    }

};


void RigidBody::updateorientation(float angle,glm::vec3 axisofrotation)
{
    float theta  = glm::radians(angle);

    float c = std::cos(theta*0.5f);
    float s = std::sin(theta*0.5f);

    glm::vec3 naxis = glm::normalize(axisofrotation);

    this->orientation = { s * naxis.x , s * naxis.y , s * naxis.z , c }; 
};

void RigidBody::genLocalInertiaMatrix()
{
    // For Cuboid Shapes Only.

    float x = 2 * this->hcubeside.x;
    float y = 2 * this->hcubeside.y;
    float z = 2 * this->hcubeside.z;

    float a = (y * y) + (z * z); 
    float b = (x * x) + (z * z); 
    float c = (y * y) + (x * x); 

    glm::mat3 ibody = {a,0.0f,0.0f,
                        0.0f,b,0.0f,
                        0.0f,0.0f,c
    };

    glm::mat3 invibody = {1.0f/a,0.0f,0.0f,
                        0.0f,1.0f/b,0.0f,
                        0.0f,0.0f,1.0f/c
    };

    this->InertiaL = ibody;
    this->invInertiaL = invibody;
    this->invmass  = 1.0f / this->mass;
    return;
                    
};

void RigidBody::updatestate(const float &dt)
{
    // Get Global Moment of Inertia Based on current orientaion.
    glm::mat3 R = glm::mat3_cast(this->orientation);
    glm::mat3 RT = glm::transpose(R);

    this->invInertiaG = R * this->invInertiaL * RT;
    this->InertiaG = R * this->InertiaL * RT;

    // Update Momentum based on collision data and force applied.
    this->amomentum = this->amomentum + this->Torque * dt;
    this->lmomentum = this->lmomentum + this->Force * dt;

    // Get Velocity and Angular Velocity From , Linear and Angular Momentum.
    this->avelocity = invInertiaG * this->amomentum;
    this->velocity = this->lmomentum * this->invmass;

    // Update Position and Orientation Based on Velocity and Angular Velocity.
    this->position = this->position + this->velocity * dt;

    glm::quat omega = {0.0f, this->avelocity.x , this->avelocity.y , this->avelocity.z};

    this->orientation = this->orientation + (0.5f * omega *this->orientation * dt );
    this->orientation = glm::normalize(this->orientation);

};
   

/////////////////////////////////////
//              Utils           ////

void Scene::showgrids(float interval)
{
    Entity* ent = this->newEntity(100);
    Mesh* m = new Mesh();
    m->gengridmesh(interval);
    ent->entitymesh = m ;
    ent->col = {0.3f,0.3f,0.3f};
    ent->isWireFrame =false;
    ent->model_matrix = glm::mat4(1.0f);
    ent->name = "Grid";

};

void Scene::stepphysics(const float &dt)
{
    //this->refreshDebugData();

    for(Entity* ent : this->entities)
    {
        if(ent->entitybody->isCollider)
        {
            ent->entitybody->updatestate(dt);
            ent->updateModelMatrix();
        }
    }
};

void Scene::gencontactdata()
{
    for (int i {0} ; i < this->entities.size() ; i++)
    {
        Entity* ent1 = this->entities[i];
        

        if (ent1->entitybody->isCollider)
        {
            for (int j =i + 1  ; j < this->entities.size() ; j++ )
            {
                Entity* ent2 = this->entities[j];
                bool collision_status = false;
                
                if (ent2->entitybody->isCollider)
                {
                    if (ent1->id == ent2->id)
                    {
                        continue;
                    }
                    else if (ent1->entitybody->s == ShapeType::cube && ent2->entitybody->s == ShapeType::cube)
                    {
                        collision_status = CollisionFunc::checkSAT(ent1->entitybody , ent2->entitybody,this->contacts,this->SATaxes);
                    }
                    else if (ent1->entitybody->s != ShapeType::cube && ent2->entitybody->s != ShapeType::cube)
                    {
                        continue;
                    }
                    else if (ent1->entitybody->s != ShapeType::cube || ent2->entitybody->s != ShapeType::cube)
                    {
                        collision_status = CollisionFunc::checkPlaneBox(ent1->entitybody,ent2->entitybody,this->contacts);
                    }

                    // if(collision_status)
                    // {
                    //     ent1->col = {1.0f,0.0f,0.0f};
                    //     ent2->col = {1.0f,0.0f,0.0f};
                        
                    // }
                    // else
                    // {
                    //     ent1->col = {1.0f,1.0f,1.0f};
                    //     ent2->col = {1.0f,1.0f,1.0f};

                    // }
                }
            }
        }
    }
}

void Scene::resolvegroundcontacts()
{
    for (Entity* ent : this->entities)
    {
        if(ent->entitybody->isCollider)
        {
            float extent = glm::length(ent->entitybody->hcubeside);
            float val = ent->entitybody->position.y - extent;

            if(val < 0.0f)
            {
                glm::vec3 colpoint = CollisionFunc::getvertexfacecontactpoint(ent->entitybody,glm::vec3{0.0,1.0f,0.0});
                glm::vec3 colnormal = glm::vec3 {0.0,1.0f,0.0};
                glm::vec3 omegaA = ent->entitybody->avelocity;
                glm::vec3 rA = colpoint - ent->entitybody->position;

                glm::vec3 velocityP = glm::cross(omegaA,rA) + ent->entitybody->velocity;

                glm::vec3 relativeP = velocityP;  
                float impulse = 0 ;
                float e  = ent->entitybody->restitution;

                if (glm::dot(relativeP,colnormal) > 0.0f)
                {
                    float linearPart = (1.0f/ent->entitybody->mass);
                    glm::vec3 A = ent->entitybody->invInertiaG * glm::cross((rA),(colnormal));
                    
                    glm::vec3 AB = glm::cross(A,rA) ;

                    float angularPart = glm::dot(AB,colnormal) ;
                    float denom = linearPart + angularPart;
                    float numer = -(1.0f + e) * (glm::dot(relativeP,colnormal));

                    impulse = numer/denom;
                }

                glm::vec3 impforce = impulse * colnormal;

                ent->entitybody->lmomentum += impforce;
                ent->entitybody->amomentum += glm::cross((rA),impforce);
                ent->entitybody->position.y += std::abs(colpoint.y);
            }
        }
    }
}


void Scene::refreshDebugData()
{
    delete this->contactmesh;
    this->contactmesh = nullptr;

    for(contact* con : this->contacts)
    {
        delete con;
        con = nullptr;
    }

    for (Mesh* mvec : this->debugvectors)
    {
        delete mvec;
        mvec = nullptr;
    }
    this->contacts.clear();
    this->debugvectors.clear();
    this->SATaxes.clear();
}

void Scene::resolveContacts()
{
    for (contact* con: this->contacts)
    {
        // Each contact will have pointer to the bodies involved in the contact.

        // First we find the velocity of contact point on each bodies.
        // for body A.
        glm::vec3 omegaA = con->bodyA->avelocity;
        glm::vec3 omegaB = con->bodyB->avelocity;
        glm::vec3 rA = con->point - con->bodyA->position;
        glm::vec3 rB = con->point - con->bodyB->position;

        glm::mat3 omegaAmat = { 0.0f ,omegaA.z ,-omegaA.y,
                            -omegaA.z,   0.0f  ,omegaA.x,
                            omegaA.y ,-omegaA.x,  0.0f  
        };

        glm::mat3 omegaBmat = { 0.0f ,omegaB.z ,-omegaB.y,
                            -omegaB.z,   0.0f  ,omegaB.x,
                            omegaB.y ,-omegaB.x,  0.0f  
        };

        glm::vec3 velocityPA = glm::cross(omegaA,rA) + con->bodyA->velocity; 
        glm::vec3 velocityPB = glm::cross(omegaB,rB) + con->bodyB->velocity;

        glm::vec3 relativePAB = velocityPA - velocityPB;  
        float impulse = 0 ;
        float e  = std::min(con->bodyA->restitution,con->bodyB->restitution);
        
        if (glm::dot(relativePAB,con->normal) > 0.0f)
        {
            float linearPart = (1.0f/con->bodyA->mass) + (1.0f/con->bodyB->mass);
            glm::vec3 A = con->bodyA->invInertiaG * glm::cross((con->point - con->bodyA->position),(con->normal));
            glm::vec3 B = con->bodyB->invInertiaG * glm::cross((con->point - con->bodyB->position),(con->normal));
            glm::vec3 AB = glm::cross(A,(con->point - con->bodyA->position)) + glm::cross(B,(con->point - con->bodyB->position));

            float angularPart = glm::dot(AB,con->normal) ;
            float denom = linearPart + angularPart;
            float numer = -(1.0f + e) * (glm::dot(relativePAB,con->normal));

            impulse = numer/denom;
        }
        glm::vec3 impforce = impulse * con->normal;

    
        if(!con->bodyA->isStatic)
        {
            con->bodyA->lmomentum += impforce;
            con->bodyA->amomentum += glm::cross((con->point - con->bodyA->position),impforce);
            
        }

        if(!con->bodyB->isStatic)
        {
            con->bodyB->lmomentum -= impforce;
            con->bodyB->amomentum -= glm::cross((con->point - con->bodyB->position),impforce);
            
        }

        if (!con->bodyA->isStatic && !con->bodyB->isStatic)
        {
            con->bodyA->position -= 0.5f * con->depth * con->normal;
            con->bodyB->position += 0.5f * con->depth * con->normal;
        }
        else if (con->bodyA->isStatic)
        {
            con->bodyB->position +=  con->depth * con->normal;
        }
        else if (con->bodyB->isStatic)
        {
            con->bodyA->position +=  con->depth * con->normal;
        }

    }
}

void Scene::drawcontacts(glm::mat4 &persp , glm::mat4 &view , glm::vec3 lightpos,int gl_primitive)
{
    this->contactmesh= new Mesh();
    contactmesh->gencuboidmesh(glm::vec3{0.1f,0.1f,0.1f});

    glUseProgram(this->shaderprogram);
    // Perspective 
    unsigned int persloc = glGetUniformLocation(this->shaderprogram,"Perspective_mat");
    glUniformMatrix4fv(persloc,1,GL_FALSE,glm::value_ptr(persp));

    // View Matrix Based on  Camera Controls
    unsigned int viewloc = glGetUniformLocation(this->shaderprogram,"View_mat");
    glUniformMatrix4fv(viewloc,1,GL_FALSE,glm::value_ptr(view));

    // Camera as lightsource
    unsigned int lightposloc = glGetUniformLocation(this->shaderprogram,"lightpos");
    glUniform3fv(lightposloc,1,glm::value_ptr(lightpos));

    for(contact* con : this->contacts)
    {

        // Model Matrix.
        glm::mat4 modelmat (1.0f);
        modelmat = modelmat * glm::translate(con->point);

        // color
        glm::vec3 color = {0.0f,1.0f,0.0f};

        // Passing Entity color as a uniform.
        unsigned int objcolloc = glGetUniformLocation(this->shaderprogram,"objcol");
        glUniform3fv(objcolloc,1,glm::value_ptr(glm::normalize(color)));

        // Passing Entity Model Matrix as a uniform. 
        unsigned int modelloc = glGetUniformLocation(this->shaderprogram,"Model_mat");
        glUniformMatrix4fv(modelloc,1,GL_FALSE,glm::value_ptr(modelmat));

        glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
        glBindVertexArray(this->contactmesh->VAO);
        glDrawArrays(gl_primitive,0,this->contactmesh->vertexcount);

        // Creating Contact vector.
        Mesh* mvec = new Mesh();
        mvec->genvectormesh(2.0f,con->normal,con->point);
        this->debugvectors.push_back(mvec);

        glm::mat4 model_mat_vec (1.0f);

        // Passing Entity Model Matrix as a uniform. 
        glUniformMatrix4fv(modelloc,1,GL_FALSE,glm::value_ptr(model_mat_vec));

        glLineWidth(3.0f);

        glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
        glBindVertexArray(mvec->VAO);
        glDrawArrays(GL_LINES,0,mvec->vertexcount);

        glLineWidth(1.0f);

        
    }
    
}

void Scene::drawobjectbasisvectors(glm::mat4 &persp , glm::mat4 &view , glm::vec3 lightpos,int gl_primitive)
{
    Mesh* xvec = new Mesh();
    Mesh* yvec = new Mesh();
    Mesh* zvec = new Mesh();
    glm::vec3 origin = {0.0f,0.0f,0.0f};
    xvec->genvectormesh(2.0f,glm::vec3 {1.0f,0.0f,0.0f},origin);
    yvec->genvectormesh(2.0f,glm::vec3 {0.0f,1.0f,0.0f},origin);
    zvec->genvectormesh(2.0f,glm::vec3 {0.0f,0.0f,1.0f},origin);

    this->debugvectors.push_back(xvec);
    this->debugvectors.push_back(yvec);
    this->debugvectors.push_back(zvec);

    glUseProgram(this->shaderprogram);
    // Perspective 
    unsigned int persloc = glGetUniformLocation(this->shaderprogram,"Perspective_mat");
    glUniformMatrix4fv(persloc,1,GL_FALSE,glm::value_ptr(persp));

    // View Matrix Based on  Camera Controls
    unsigned int viewloc = glGetUniformLocation(this->shaderprogram,"View_mat");
    glUniformMatrix4fv(viewloc,1,GL_FALSE,glm::value_ptr(view));

    // Camera as lightsource
    unsigned int lightposloc = glGetUniformLocation(this->shaderprogram,"lightpos");
    glUniform3fv(lightposloc,1,glm::value_ptr(lightpos));

    for (Entity* ent : this->entities)
    {
        glm::mat4 modelmat = ent->model_matrix;

        if(ent == this->scene_bound)
        {// using this entity to draw global basis vectors.
            modelmat = glm::mat4 (1.0f);
            
        }
        glm::vec3 colx = {1.0f,0.0f,0.0f};
        glm::vec3 coly = {0.0f,1.0f,0.0f};
        glm::vec3 colz = {0.0f,0.0f,1.0f};

        

        // Passing Entity Model Matrix as a uniform. 
        unsigned int modelloc = glGetUniformLocation(this->shaderprogram,"Model_mat");
        glUniformMatrix4fv(modelloc,1,GL_FALSE,glm::value_ptr(modelmat));

        // Passing Entity color as a uniform.
        unsigned int objcolloc = glGetUniformLocation(this->shaderprogram,"objcol");
        
        //std::cout << "Drawing Basis Vectors" << std::endl;
        glLineWidth(3.0f);
        glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

        glUniform3fv(objcolloc,1,glm::value_ptr(glm::normalize(colx)));

        glBindVertexArray(xvec->VAO);
        glDrawArrays(GL_LINES,0,xvec->vertexcount);

        glUniform3fv(objcolloc,1,glm::value_ptr(glm::normalize(coly)));

        glBindVertexArray(yvec->VAO);
        glDrawArrays(GL_LINES,0,yvec->vertexcount);

        glUniform3fv(objcolloc,1,glm::value_ptr(glm::normalize(colz)));

        glBindVertexArray(zvec->VAO);
        glDrawArrays(GL_LINES,0,yvec->vertexcount);
        glLineWidth(1.0f);

    }
}




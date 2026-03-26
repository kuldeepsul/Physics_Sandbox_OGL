#include "shapes.h"
#define PI 3.14159265358979323846


void mesh::genBufferObjects()
{
    // generate
    glGenVertexArrays(1,&this->VAO);
    glGenBuffers(1,&this->VBO);
    
    // bind
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER,VBO);
    
    // specify attributes
    glBufferData(GL_ARRAY_BUFFER,this->data.size() * sizeof(float), this->data.data(),GL_STATIC_DRAW);
    

    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,6*sizeof(float),(void*)(0*sizeof(float)));
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,6*sizeof(float),(void*)(3*sizeof(float)));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    
};

void mesh::gencuboidmesh(glm::vec3 sides)
{

    float hx = sides.x * 0.5f;
    float hy = sides.y *  0.5f;
    float hz = sides.z * 0.5f;

    std::vector<float> vertexData = {
        // FRONT (+Z)
        -hx, -hy,  hz,  0, 0, 1,
        hx, -hy,  hz,  0, 0, 1,
        hx,  hy,  hz,  0, 0, 1,

        hx,  hy,  hz,  0, 0, 1,
        -hx,  hy,  hz,  0, 0, 1,
        -hx, -hy,  hz,  0, 0, 1,

        // BACK (-Z)  ← flipped compared to yours
        hx, -hy, -hz,  0, 0,-1,
        -hx, -hy, -hz,  0, 0,-1,
        -hx,  hy, -hz,  0, 0,-1,

        -hx,  hy, -hz,  0, 0,-1,
        hx,  hy, -hz,  0, 0,-1,
        hx, -hy, -hz,  0, 0,-1,

        // LEFT (-X)
        -hx, -hy, -hz, -1, 0, 0,
        -hx, -hy,  hz, -1, 0, 0,
        -hx,  hy,  hz, -1, 0, 0,

        -hx,  hy,  hz, -1, 0, 0,
        -hx,  hy, -hz, -1, 0, 0,
        -hx, -hy, -hz, -1, 0, 0,

        // RIGHT (+X)
        hx, -hy,  hz,  1, 0, 0,
        hx, -hy, -hz,  1, 0, 0,
        hx,  hy, -hz,  1, 0, 0,

        hx,  hy, -hz,  1, 0, 0,
        hx,  hy,  hz,  1, 0, 0,
        hx, -hy,  hz,  1, 0, 0,

        // TOP (+Y)
        -hx,  hy,  hz,  0, 1, 0,
        hx,  hy,  hz,  0, 1, 0,
        hx,  hy, -hz,  0, 1, 0,

        hx,  hy, -hz,  0, 1, 0,
        -hx,  hy, -hz,  0, 1, 0,
        -hx,  hy,  hz,  0, 1, 0,

        // BOTTOM (-Y)
        -hx, -hy, -hz,  0,-1, 0,
        hx, -hy, -hz,  0,-1, 0,
        hx, -hy,  hz,  0,-1, 0,

        hx, -hy,  hz,  0,-1, 0,
        -hx, -hy,  hz,  0,-1, 0,
        -hx, -hy, -hz,  0,-1, 0
    };

    
    this->data =  vertexData;
    this->vertexcount = 36;
    this->genBufferObjects();

};

void mesh::genplanemesh(float a , float b)
{
    float hx  = a * 0.5f;
    float hz  = b * 0.5f;

    std::vector<float> vertexData = {
        
        -hx, 0.0f, -hz,  0.0f,  1.0f,  0.0f,
        hx , 0.0f, -hz,  0.0f,  1.0f,  0.0f,
        hx , 0.0f,  hz,  0.0f,  1.0f,  0.0f,

        -hx, 0.0f, -hz,  0.0f,  1.0f,  0.0f,
         hx , 0.0f,  hz,  0.0f,  1.0f,  0.0f,
        -hx, 0.0f,  hz,  0.0f,  1.0f,  0.0f
    };

    
    this->data = vertexData;
    this->vertexcount = 6 ;
    this->genBufferObjects();

};

void mesh::readobj(std::string path)
{
    // Creating full path from reletive path. 
    std::filesystem::path root = std::filesystem::path(__FILE__).parent_path().parent_path();
    std::string fullpath = (root/path).string();

    // Reading file 
    meshio temp;
    std::string line ;

    std::fstream file (fullpath);

    // checking if the file opened properly.
    if(!file.is_open())
    {
        throw std::runtime_error("FATAL ERROR : .obj file path not found.");
    }

    while(std::getline(file,line))
    {
        std::stringstream s (line);
        std::string value ;
        
        std::getline(s,value,' ');

        if(value == "v")
        {
            readvertex(temp,s);
        }
        else if (value == "vn")
        {
            readnormal(temp,s);
        }
        else if (value == "f")
        {
            readface(temp,s);
        }
        else if (value == "s" || value == "0")
        {
            continue;
        }
        
    }

    
    this->data = temp.bufferdata;
    int count = this->data.size()/6;
    this->vertexcount = count;
    this->genBufferObjects();

};

void mesh::readvertex (meshio& iotemp ,std::stringstream &s)
{
    float x,y,z;
    std::string val;

    std::getline(s,val,' ');
    x  = std::stof(val);

    std::getline(s,val,' ');
    y  = std::stof(val);
        
    std::getline(s,val,' ');
    z  = std::stof(val);

    iotemp.positiondata.push_back(x);
    iotemp.positiondata.push_back(y);
    iotemp.positiondata.push_back(z);

};

void mesh::readnormal (meshio& iotemp ,std::stringstream &s)
{
    float x,y,z;
    std::string val;

    std::getline(s,val,' ');
    x  = std::stof(val);

    std::getline(s,val,' ');
    y  = std::stof(val);
        
    std::getline(s,val,' ');
    z  = std::stof(val);
    
    iotemp.normaldata.push_back(x);
    iotemp.normaldata.push_back(y);
    iotemp.normaldata.push_back(z);
};

void mesh::readface (meshio& iotemp ,std::stringstream &s)
{
    std::string entry;

    while(std::getline(s,entry,' '))
    {
        std::stringstream s1(entry);
        std::string val; 
        int v,t,n;

        std::getline(s1,val,'/');
        v  = std::stoi(val);

        std::getline(s1,val,'/');
        //t  = std::stoi(val);
            
        std::getline(s1,val,'/');
        n  = std::stoi(val);

        int vindex = v - 1 ;
        int nindex = n - 1 ;

        iotemp.bufferdata.push_back(iotemp.positiondata[vindex*3]);
        iotemp.bufferdata.push_back(iotemp.positiondata[vindex*3 + 1]);
        iotemp.bufferdata.push_back(iotemp.positiondata[vindex*3 + 2]);

        iotemp.bufferdata.push_back(iotemp.normaldata[nindex*3 ]);
        iotemp.bufferdata.push_back(iotemp.normaldata[nindex*3 + 1]);
        iotemp.bufferdata.push_back(iotemp.normaldata[nindex*3 + 2]);
    }
    
};


Entity* Scene::newEntity(unsigned int id)
{
    Entity* ent = new Entity(id);
    this->entities.push_back(ent);

    return ent;
}

void Scene::newEntity(unsigned int id , shapetype s , glm::vec3 sides)
{
    Entity* ent = new Entity(id);

    // Generete mesh and attach to entity
    mesh* m = new mesh(); 
    
    m->gencuboidmesh(sides);
    ent->entitymesh = m;

    // Generete rigid body and attach to entity.
    ent->entitybody = new rigidbody(shapetype::cube,sides);
    
    this->entities.push_back(ent);
};

void Entity::translateEntity(glm::vec3 disp)
{
    this->model_matrix =  glm::translate(this->model_matrix , disp);
};

void Entity::rotateEntity(float angle, glm::vec3 axis)
{
    this->model_matrix =  glm::rotate(this->model_matrix,angle,axis);
};

void Entity::scaleEntity(glm::vec3 axis)
{
    this->model_matrix =  glm::scale(this->model_matrix,axis);
};

void Entity::updateModelMatrix()
{
    this->model_matrix = glm::translate(glm::mat4 (1.0f) ,this->entitybody->position);
};

rigidbody::rigidbody(shapetype s_param,float radii)
{
    if(s_param != shapetype::sphere)
    {
        throw std::runtime_error("FATAL ERROR: Invalid rigid body parameters.");
    }

    this->radius = radii;
    this->mass = radii;
};

rigidbody::rigidbody(shapetype s_param,glm::vec3 side)
{
    if(s_param != shapetype::cube)
    {
        throw std::runtime_error("FATAL ERROR: Invalid rigid body parameters.");
    }
    this->hcubeside = 0.5f * side;
};

rigidbody::rigidbody(shapetype s_param,glm::vec2 sides , glm::vec3 norm)
{
    if(s_param != shapetype::plane)
    {
        throw std::runtime_error("FATAL ERROR: Invalid rigid body parameters.");
    }
    this->hplaneside = sides;
    this->normplane = norm;
};

void rigidbody::checkboundcollision(bound &domain)
{
    // Lower bounds .
    if (this->position.y - this->hcubeside.y  < domain.negybound)
    {
        this->position.y = domain.negybound + this->hcubeside.y ;
        this->velocity.y = -this->velocity.y ;
    }
    // Upper bounds.
    else if (this->position.y + this->hcubeside.y > domain.posybound)
    {
        this->position.y = domain.posybound - this->hcubeside.y ;
        this->velocity.y = -this->velocity.y ;
    }

    // Left bounds .
    if (this->position.x - this->hcubeside.x < domain.negxbound)
    {
        this->position.x = domain.negxbound + this->hcubeside.x ;
        this->velocity.x = -this->velocity.x ;
        
    }
    // Right bounds.
    else if (this->position.x + this->hcubeside.x > domain.posxbound)
    {
        this->position.x = domain.posxbound - this->hcubeside.x ;
        this->velocity.x = -this->velocity.x ;      
    }

    // Front bounds .
    if (this->position.z - this->hcubeside.z < domain.negzbound)
    {
        this->position.z = domain.negzbound + this->hcubeside.z ;
        this->velocity.z = -this->velocity.z ;
    }
    // Rear bounds.
    else if (this->position.z + this->hcubeside.z > domain.poszbound)
    {
        this->position.z = domain.poszbound - this->hcubeside.z ;
        this->velocity.z = -this->velocity.z ;
    }
};

void rigidbody::checkAABB(rigidbody* other)
{
    // check overlap
    float penx = 0.0f ;
    float peny = 0.0f ;
    float penz = 0.0f ; 

    bool overlapx {false};
    bool overlapy {false};
    bool overlapz {false};

    // in X 

    if (this->position.x >  other->position.x)
    {
        penx = (other->position.x + other->hcubeside.x) - (this->position.x - this->hcubeside.x);

        if (penx > 0)
        {
            // other->position.x  -= 0.5*penx;
            // this->position.x += 0.5*penx;
            overlapx = true ;
        }
    }
    else
    {
        penx = (this->position.x + this->hcubeside.x) - (other->position.x - other->hcubeside.x) ;

        if (penx > 0)
        {
            // this->position.x -= 0.5 * penx; 
            // other->position.x += 0.5 * penx;          
            overlapx = true ;
        }
    }

    // in Y
    if (this->position.y >  other->position.y)
    {
        peny = (other->position.y + other->hcubeside.y) - (this->position.y - this->hcubeside.y);

        if (peny > 0)
        {
            // other->position.y  -= 0.5*peny;
            // this->position.y += 0.5*peny;
            overlapy = true ;
        }
    }
    else
    {
        peny = (this->position.y + this->hcubeside.y) - (other->position.y - other->hcubeside.y) ;

        if (peny > 0)
        {
            // this->position.y -= 0.5 * peny; 
            // other->position.y += 0.5 * peny; 
            overlapy = true ;
        }
    }

    // in Z
    if (this->position.z >  other->position.z)
    {
        penz = (other->position.z + other->hcubeside.z) - (this->position.z - this->hcubeside.z);

        if (penz > 0)
        {
            // other->position.z  -= 0.5*penz;
            // this->position.z += 0.5*penz;
            overlapz = true ;
        }
    }
    else
    {
        penz = (this->position.z + this->hcubeside.z) - (other->position.z - other->hcubeside.z) ;

        if (penz > 0)
        {
            // this->position.z -= 0.5 * penz; 
            // other->position.z += 0.5 * penz; 
            overlapz = true ;
        }
    }

    if (overlapx && overlapy && overlapz)
    {
        float m1 = this->mass;
        float m2 = other->mass;

        // Collision resolution. 

        if (penx < peny && penx < penz)
        {
            float vi1 = this->velocity.x;
            float vi2 = other->velocity.x;

            this->velocity.x = ((m1-m2)/(m1+m2))*vi1 + ((2*m2)/(m1 + m2))* vi2;
            other->velocity.x =((m2-m1)/(m1+m2))*vi2 + ((2*m1)/(m1 + m2))* vi1;
        }
        else if (peny < penx && peny < penz)
        {
            float vi1 = this->velocity.y;
            float vi2 = other->velocity.y;

            this->velocity.y = ((m1-m2)/(m1+m2))*vi1 + ((2*m2)/(m1 + m2))* vi2;
            other->velocity.y =((m2-m1)/(m1+m2))*vi2 + ((2*m1)/(m1 + m2))* vi1;
        }
        else if (penz < penx && penz < peny)
        {
            float vi1 = this->velocity.z;
            float vi2 = other->velocity.z;

            this->velocity.z = ((m1-m2)/(m1+m2))*vi1 + ((2*m2)/(m1 + m2))* vi2;
            other->velocity.z =((m2-m1)/(m1+m2))*vi2 + ((2*m1)/(m1 + m2))* vi1;
        }
    }
};
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

mesh MeshGenerator::gencuboidmesh(glm::vec3 sides)
{

    float hx = sides.x;
    float hy = sides.y;
    float hz = sides.z;

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

    mesh obj;
    obj.data =  vertexData;
    obj.vertexcount = 36;
    obj.genBufferObjects();

    return obj;

};

mesh MeshGenerator::genplanemesh(float a , float b)
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

    mesh obj;
    obj.data = vertexData;
    obj.vertexcount = 6 ;
    obj.genBufferObjects();

    return obj;
};

mesh MeshGenerator::readobj(std::string path)
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

    mesh obj;
    obj.data = temp.bufferdata;
    int count = obj.data.size()/6;
    obj.vertexcount = count;
    obj.genBufferObjects();

    return obj;

};

void MeshGenerator::readvertex (meshio& iotemp ,std::stringstream &s)
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

void MeshGenerator::readnormal (meshio& iotemp ,std::stringstream &s)
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

void MeshGenerator::readface (meshio& iotemp ,std::stringstream &s)
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


void Scene::newEntity(unsigned int id ,mesh* mesh_data,rigidbody* bodydata)
{
    Entity* ent = new Entity(id);
    ent->entitymesh = mesh_data;
    ent->entitybody = bodydata;
    
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
};

rigidbody::rigidbody(shapetype s_param,glm::vec3 side)
{
    if(s_param != shapetype::cube)
    {
        throw std::runtime_error("FATAL ERROR: Invalid rigid body parameters.");
    }
    this->hcubeside = side;
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
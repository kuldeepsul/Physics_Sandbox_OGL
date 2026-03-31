#include "shapes.h"
#define PI 3.14159265358979323846

/////////////////////////////////////////////////////////////
//////////        Mesh Generation  function /////////////////

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

/////////////////////////////////////////////////////////////////



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

void Entity::updateModelMatrix()
{
    glm::mat4 translation_matrix = glm::translate(glm::mat4 (1.0f) ,this->entitybody->position);

    glm::mat4 rotation_matrix = glm::mat4_cast(this->entitybody->orientation);
    this->model_matrix = translation_matrix * rotation_matrix ;
};


rigidbody::rigidbody(shapetype s_param,glm::vec3 side)
{
    if(s_param != shapetype::cube)
    {
        throw std::runtime_error("FATAL ERROR: Invalid rigid body parameters.");
    }
    this->hcubeside = 0.5f * side;
};

void rigidbody::checkboundcollision(rigidbody* domain)
{
    glm::vec3 HEbody = this->position + hcubeside;
    glm::vec3 LEbody = this->position - hcubeside;

    glm::vec3 HEbound = domain->position + domain->hcubeside;
    glm::vec3 LEbound = domain->position - domain->hcubeside;

    // Lower bounds .
    if (LEbody.y < LEbound.y)
    {
        this->position.y = LEbound.y + this->hcubeside.y ;
        this->velocity.y = -this->velocity.y ;
    }
    // Upper bounds.
    else if (HEbody.y > HEbound.y)
    {
        this->position.y = HEbound.y - this->hcubeside.y ;
        this->velocity.y = -this->velocity.y ;
    }

    // Left bounds .
    if (LEbody.x < LEbound.x)
    {
        this->position.x = LEbound.x + this->hcubeside.x ;
        this->velocity.x = -this->velocity.x ;
        
    }
    // Right bounds.
    else if (HEbody.x > HEbound.x)
    {
        this->position.x =HEbound.x - this->hcubeside.x ;
        this->velocity.x = -this->velocity.x ;      
    }

    // Front bounds .
    if (LEbody.z < LEbound.z)
    {
        this->position.z = LEbound.z + this->hcubeside.z ;
        this->velocity.z = -this->velocity.z ;
    }
    // Rear bounds.
    else if (HEbody.z > HEbound.z)
    {
        this->position.z = HEbound.z - this->hcubeside.z ;
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
            //other->position.x  -= 0.5*penx;
            //this->position.x += 0.5*penx;
            overlapx = true ;
        }
    }
    else
    {
        penx = (this->position.x + this->hcubeside.x) - (other->position.x - other->hcubeside.x) ;

        if (penx > 0)
        {
            //this->position.x -= 0.5 * penx; 
            //other->position.x += 0.5 * penx;          
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

std::vector <glm::vec3> rigidbody::getvertexdata()
{
    glm::vec3 hside = this->hcubeside;

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


    glm::mat4 rotation_matrix = glm::mat4_cast(this->orientation);

    vertexdata.push_back(glm::mat3(rotation_matrix)*vert1 + this->position);
    vertexdata.push_back(glm::mat3(rotation_matrix)*vert2 + this->position);
    vertexdata.push_back(glm::mat3(rotation_matrix)*vert3 + this->position);
    vertexdata.push_back(glm::mat3(rotation_matrix)*vert4 + this->position);
    vertexdata.push_back(glm::mat3(rotation_matrix)*vert5 + this->position);
    vertexdata.push_back(glm::mat3(rotation_matrix)*vert6 + this->position);
    vertexdata.push_back(glm::mat3(rotation_matrix)*vert7 + this->position);
    vertexdata.push_back(glm::mat3(rotation_matrix)*vert8 + this->position);


    
    
   

    // Apply World Transforms .
    for (int i {0} ; i < vertexdata.size() ; i++)
    {
        std::cout<< vertexdata[i].x << "," << vertexdata[i].y << "," << vertexdata[i].z << "," << std::endl;
    }


    return vertexdata;

}

std::vector <glm::vec3> rigidbody::getSATaxes(const std::vector <glm::vec3> &vertdataA ,const std::vector <glm::vec3> &vertdataB) const
{
    glm::vec3 normA1 =  vertdataA[0] - vertdataA[4];
    glm::vec3 normA2 =  vertdataA[1] - vertdataA[0];
    glm::vec3 normA3 =  vertdataA[3] - vertdataA[0];

    
    glm::vec3 normB1 =  vertdataB[0] - vertdataB[4];
    glm::vec3 normB2 =  vertdataB[1] - vertdataB[0];
    glm::vec3 normB3 =  vertdataB[3] - vertdataB[0];

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
        // Cross Axes
    };

    // Cross Normals.
    for (auto& vecA : norm_cubeA)
    {
        for (auto vecB : norm_cubeB)
        {
            glm::vec3 temp = glm::cross(vecA,vecB);
            if(temp.x < 1.0e-12 && temp.x < 1.0e-12 && temp.x < 1.0e-12 )
            {
                ///std::cout << "Zero axis found" << std::endl;
                temp = vecA;
                SATaxes.push_back(temp);
                continue;
            }
            else
            {
                SATaxes.push_back(glm::normalize(temp));
            }
            
        }
    }
    //std::cout << "SAT axed formed " << std::endl;
    return SATaxes;

}

glm::vec2 rigidbody::getMinMaxprojection(const glm::vec3 &axis, const std::vector <glm::vec3> & vertexdata) const
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
    // std::cout << std::endl;
    
    // std::cout << "Min : " << min << " , Max : " << max << std::endl;

    return glm::vec2(min,max);
}

float rigidbody::getaxispenetration (const glm::vec3& axis, const std::vector <glm::vec3>& vertdataA ,const std::vector <glm::vec3>& vertdataB) const
{
    std::cout << "Penetration Detection per axis." << std::endl;

    glm::vec2 projA = rigidbody::getMinMaxprojection(axis,vertdataA);
    glm::vec2 projB = rigidbody::getMinMaxprojection(axis,vertdataB);

    std::cout << "Complete!" << std::endl;

    float penetration = 0 ;
    if (projA.x <= projB.x)
    {
        penetration = projB.x - projA.y;
    }
    else if (projB.x <= projA.x)
    {
        penetration = projA.x - projB.y;
    }

    //std::cout << penetration << std::endl;
    return penetration;
}

bool rigidbody::checkSAT(rigidbody* other)
{
    std::cout << "Checking sat collisions" << std::endl;
    std::vector <glm::vec3> vertdataA ;
    std::vector <glm::vec3> vertdataB ;

    vertdataA = this->getvertexdata();
    vertdataB = other->getvertexdata();
    

    // Create Normal of faces for SAT Checks.
    
    std::vector <glm::vec3> SATaxes = this->getSATaxes(vertdataA,vertdataB);
    std::vector <float> axispen ; 
    
    
    
    int k = 1 ;
    for (auto& axis  : SATaxes)
    {

        float temp = rigidbody::getaxispenetration(axis,vertdataA,vertdataB);
        if (temp >= 0)
        {
            axispen.push_back(-1.0f);
        }
        else if (temp < 0)
        {
            axispen.push_back(-temp);
        }
    }

    

    bool collision = true;
    float min_collision = 0 ;
    int collision_axis = -1 ;
    int i {0} ;

    
    while (collision)
    {
        if( i == axispen.size())
        {
            break;
        }

        if (axispen[i] < 0.0f)
        {
            collision = false;
        }
        else 
        {
           
            if( i == 0)
            {
                min_collision = axispen[i];
            }
            else 
            {
                if (min_collision  > axispen[i])
                {
                    min_collision = axispen[i];
                    collision_axis = i ;
                }
            }
            i++;
        }
    }
    if (collision)
    {
        std::cout << "collision detected!" << std::endl;
    }
    else
    {
        std::cout << "--" << std::endl;
    }
    return collision;

}

void rigidbody::updateorientation(float angle,glm::vec3 axisofrotation)
{
    float theta  = glm::radians(angle);

    float c = std::cos(theta*0.5f);
    float s = std::sin(theta*0.5f);

    glm::vec3 naxis = glm::normalize(axisofrotation);

    this->orientation = { s * naxis.x , s * naxis.y , s * naxis.z , c }; 
};

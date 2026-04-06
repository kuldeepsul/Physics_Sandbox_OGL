#include "shapes.h"
#define PI 3.14159265358979323846

///////////////////////////////////////////////////////////
//////          Geometry Helper function        //////////

static float ClosestPtSegementSegment                       // Return Distance squared between the closest point on segement.
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
            
    t =( (b * s) + f) / c ;

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




////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
///////            Obj File Reader Functions        ////////

void ObjReader::ReadMeshFromObjFile(Mesh* object ,std::string path)
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
            ReadVertexData(temp,s);
        }
        else if (value == "vn")
        {
            ReadNormalsData(temp,s);
        }
        else if (value == "f")
        {
            ReadFaceData(temp,s);
        }
        else if (value == "s" || value == "0")
        {
            continue;
        }
        
    }

    
    object->data = temp.bufferdata;
    int count = object->data.size()/6;
    object->vertexcount = count;
    object->genBufferObjects();
}

void ObjReader::ReadVertexData (meshio& iotemp ,std::stringstream &s)
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

void ObjReader::ReadNormalsData (meshio& iotemp ,std::stringstream &s)
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

void ObjReader::ReadFaceData (meshio& iotemp ,std::stringstream &s)
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

////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
//////////        Mesh Generation  function /////////////////

void Mesh::genBufferObjects()
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

void Mesh::gencuboidmesh(glm::vec3 sides)
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

void Mesh::gengridmesh(float interval)
{
    float size = 100.0f;
    int samples = size / interval ;
    float hsize = size * 0.5f ;

    glm::vec3 origin  = {0.0f,0.0f,0.0f};
    glm::vec3 lextent = origin - glm::vec3 {hsize,0.0f,hsize};
    glm::vec3 hextent = origin + glm::vec3 {hsize,0.0f,hsize};

    std::vector <float> vertexdata;
    int count = 0 ;
    
    for (int i {0} ; i <= samples ; i++)
    {
        glm::vec3 xinc1 = lextent + glm::vec3 {interval*i,0.0f,0.0f};
        glm::vec3 xinc2 = lextent + glm::vec3 {interval*i,0.0f,size};

        glm::vec3 zinc1 = lextent + glm::vec3 {0.0f,0.0f,interval*i};
        glm::vec3 zinc2 = lextent + glm::vec3 {size,0.0f,interval*i};

        vertexdata.push_back(xinc1.x);
        vertexdata.push_back(xinc1.y);
        vertexdata.push_back(xinc1.z);
        vertexdata.push_back(xinc2.x);
        vertexdata.push_back(xinc2.y);
        vertexdata.push_back(xinc2.z);

        vertexdata.push_back(zinc1.x);
        vertexdata.push_back(zinc1.y);
        vertexdata.push_back(zinc1.z);
        vertexdata.push_back(zinc2.x);
        vertexdata.push_back(zinc2.y);
        vertexdata.push_back(zinc2.z);

        count += 4;
        
    }
    
    // vertexdata = { -20.0f, 30.0f ,20.0f,
    //                 -20.0f , 30.0f ,20.f,
    //                 20.0f,30.0f,20.0f};

    this->data = vertexdata;
    this->vertexcount = count;

    glGenBuffers(1,&this->VBO);
    glGenVertexArrays(1,&this->VAO);

    glBindVertexArray(this->VAO);
    glBindBuffer(GL_ARRAY_BUFFER,this->VBO);
    

    glBufferData(GL_ARRAY_BUFFER,this->data.size()*sizeof(float),this->data.data(),GL_STATIC_DRAW);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float),0);
    glEnableVertexAttribArray(0);

}

void Mesh::genvectormesh(const float& mag , const glm::vec3& dir , const glm::vec3& position)
{
    glm::vec3 start = position;
    glm::vec3 end = position + mag * dir ;
    this->data = { start.x  , start.y , start.z ,
                   end.x    , end.y   , end.z  
    };
    this->vertexcount = 2 ;
    

    glGenVertexArrays(1,&this->VAO);
    glGenBuffers(1,&this->VBO);

    glBindVertexArray(this->VAO);
    glBindBuffer(GL_ARRAY_BUFFER , this->VBO);

    glBufferData(GL_ARRAY_BUFFER,this->data.size() * sizeof(float), this->data.data(),GL_STATIC_DRAW);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float),0);
    glEnableVertexAttribArray(0);

        
}

void Mesh::genfromobj(std::string path)
{
    ObjReader::ReadMeshFromObjFile(this,path);
};

/////////////////////////////////////////////////////////////////

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

int CollisionFunc::checkvertexinclusion(const std::vector <glm::vec3> &cubedataA , const std::vector <glm::vec3> &cubedataB, const int &axis_id , const glm::vec3 &axis )
{
    float min , max ;
 
    if (axis_id == 0)
    {
        min = glm::dot(cubedataA[4],axis);
        max = glm::dot(cubedataA[0],axis);
    }
    else if (axis_id == 1)
    {
        min = glm::dot(cubedataA[0],axis);
        max = glm::dot(cubedataA[1],axis);
    }
    else if (axis_id == 2)
    {
        min = glm::dot(cubedataA[0],axis);
        max = glm::dot(cubedataA[3],axis);
    }


    for (int i {0}; i < 8 ; i ++ )
    {
        float val = glm::dot(cubedataB[i],axis);

        if( val < max && val > min)
        {
           return i ;
        }        
    }
    return -1;
}

std::pair <std::pair<int,int>,std::pair<int,int>> getcollisionedges(const std::vector <glm::vec3> &vertdataA , 
                                                                                const std::vector <glm::vec3> &vertdataB , 
                                                                                const glm::vec3 &axis , 
                                                                                const int &axis_id
)
{
    
}


glm::vec3 CollisionFunc::getcontactpoint(const std::vector <glm::vec3> &vertdataA , const std::vector <glm::vec3> &vertdataB , const glm::vec3 &axis , const int &axis_id)
{
    int vertid;
    if (axis_id < 6)
    {
        // its a face to vertex collision.
        if(axis_id < 3)
        {
            // vertex of second cube is penetrating into first cube.
            vertid = checkvertexinclusion(vertdataA,vertdataB,axis_id,axis);
            if(vertid == -1)
            {
                throw std::runtime_error("Unable to find collision point");
            }
            return vertdataB[vertid];
        }
        else
        {
            // vertex of first cube is penetrating into second cube.
            vertid = checkvertexinclusion(vertdataA,vertdataB,axis_id - 3,axis);
            if(vertid == -1)
            {
                throw std::runtime_error("Unable to find collision point");
            }
            return vertdataA[vertid];
        }
        
    }
    else
    {
        // its a edge to edge collision.
        std::pair <std::pair<int,int> , std::pair<int,int>> edges_id;
        edges_id = getcollisionedges(vertdataA,vertdataB,axis,axis_id);

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

std::vector <glm::vec3> CollisionFunc::getSATaxes(const std::vector <glm::vec3> &vertdataA ,const std::vector <glm::vec3> &vertdataB) 
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
    //std::cout << "SAT axed formed " << std::endl;
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

bool CollisionFunc::checkSAT(RigidBody* body1 , RigidBody* body2)
{
    std::vector <glm::vec3> vertdataA ;
    std::vector <glm::vec3> vertdataB ;

    vertdataA = getvertexdata(body1);
    vertdataB = getvertexdata(body2);
    

    // Create Normal of faces for SAT Checks.
    
    std::vector <glm::vec3> SATaxes = getSATaxes(vertdataA,vertdataB);
    bool collision = true ;
    float min_collision = 0;
    glm::vec3 collisionnormal;
    
    for (int i {0} ; i < SATaxes.size() ; i++)
    {
        glm::vec2 projA = getMinMaxprojection(SATaxes[i],vertdataA);
        glm::vec2 projB = getMinMaxprojection(SATaxes[i],vertdataB);

        //std::cout << "Complete!" << std::endl;

        float penetration = 0 ;
        if (projA.x > projB.y || projB.x > projA.y)
        {
            return false;
        }

        penetration = std::min (projA.y,projB.y) - std::max (projB.x ,projA.x);
        
        if (penetration < min_collision)
        {
            min_collision = penetration;
            collisionnormal = SATaxes[i];
        }
        
    }



    contact conA , conB;
    conA.normal = collisionnormal;
    conB.normal = -collisionnormal;
    conA.point = conB.point = {0.0f,0.0f,0.0f};
    conA.depth = min_collision;
    conB.depth = min_collision;

    body1->cons.push_back(conA);
    body2->cons.push_back(conB);
    
    return collision;

}

/////////////////////////////////////////////////////////////////////



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

void Scene::drawScene(glm::mat4 &persp , glm::mat4 &view , glm::vec3 lightpos)
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
            glDrawArrays(GL_TRIANGLES,0,ent->entitymesh->vertexcount);
        }
        else
        {
            glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
            glBindVertexArray(ent->entitymesh->VAO);
            glDrawArrays(GL_TRIANGLES,0,ent->entitymesh->vertexcount);
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
    if(s_param != ShapeType::cube)
    {
        throw std::runtime_error("FATAL ERROR: Invalid rigid body parameters.");
    }
    this->hcubeside = 0.5f * side;
};

void RigidBody::resolvecontacts()
{
    this->cons.clear();
}

void RigidBody::updateorientation(float angle,glm::vec3 axisofrotation)
{
    float theta  = glm::radians(angle);

    float c = std::cos(theta*0.5f);
    float s = std::sin(theta*0.5f);

    glm::vec3 naxis = glm::normalize(axisofrotation);

    this->orientation = { s * naxis.x , s * naxis.y , s * naxis.z , c }; 
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

void Scene::showcontacts(RigidBody* body)
{


    for ( const contact &c : body->cons )
    {
        Mesh* vecmesh ;
        vecmesh->genvectormesh(1.0f,c.normal,body->position);
        glm::mat4 model_matrix (1.0f);

        // Passing Entity Model Matrix as a uniform. 
        unsigned int modelloc = glGetUniformLocation(this->shaderprogram,"Model_mat");
        glUniformMatrix4fv(modelloc,1,GL_FALSE,glm::value_ptr(model_matrix));

        glBindVertexArray(vecmesh->VAO);
        glDrawArrays(GL_LINES,0,vecmesh->vertexcount);
        
    }

}


////////////////////////////////////////////////////
///////             Shader Function         /////////
///////////////////////////////////////////////////

std::string readShaderFile(std::string path)
{
    std::fstream file (path);

    if(!file.is_open())
    {
        throw std::runtime_error("FATAL ERROR : Shader Path not found");
    }

    std::stringstream s;
    s << file.rdbuf();
    std::string shadercode = s.str();

    return shadercode;
}

void checkShaderCompilation(unsigned int &shader)
{
    int success;
    char infolog [512];

    glGetShaderiv(shader,GL_COMPILE_STATUS,&success);

    if(!success)
    {
        glGetShaderInfoLog(shader,512,nullptr,infolog);
        std::cout << "Shader Compilation Error : " << infolog << std::endl;
    }
}

unsigned int getShaderProgram(std::string& path_vert,std::string path_frag)
{
    unsigned int fragment_shader;
    unsigned int vertex_shader;
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

    std::filesystem::path root = std::filesystem::path(__FILE__).parent_path().parent_path();

    std::string frag_path = (root/path_frag).string();
    std::string vert_path = (root/path_vert).string();

    std::string vertcode = readShaderFile(vert_path);
    std::string fragcode = readShaderFile(frag_path);

    const char* vertsource = vertcode.c_str();
    const char* fragsource = fragcode.c_str();

    glShaderSource(vertex_shader,1,&vertsource,nullptr);
    glShaderSource(fragment_shader,1,&fragsource,nullptr);

    glCompileShader(vertex_shader);
    glCompileShader(fragment_shader);

    checkShaderCompilation(vertex_shader);
    checkShaderCompilation(fragment_shader);

    unsigned int program;
    program = glCreateProgram();

    glAttachShader(program,vertex_shader);
    glAttachShader(program,fragment_shader);

    glLinkProgram(program);

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    return program;


};

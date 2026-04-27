#include "objreader.h"

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

void Mesh::genplanemesh(float size)
{
    float hsize  = size * 0.5f;

    // First we will create a general flat ground plane whose normal is y-axis.

    std::vector <float> vertexData = 
    {
        hsize,0.0f,hsize,0.0f,1.0f,0.0f,
        -hsize,0.0f,hsize,0.0f,1.0f,0.0f,
        hsize,0.0f,-hsize,0.0f,1.0f,0.0f,

        hsize,0.0f,-hsize,0.0f,1.0f,0.0f,
        -hsize,0.0f,hsize,0.0f,1.0f,0.0f,
        -hsize,0.0f,-hsize,0.0f,1.0f,0.0f,

    };

    // Now we will rotate the data as per the normal.


    this->data =  vertexData;
    this->vertexcount = 6;
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

void Mesh::genbasismesh()
{
    this->data = {0.0f,0.0f,0.0f,1.0f,0.0f,0.0f,
                0.0f,0.0f,0.0f,0.0f,1.0f,0.0f,
                0.0f,0.0f,0.0f,0.0f,0.0f,1.0f
    };

    this->vertexcount = 6 ;

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
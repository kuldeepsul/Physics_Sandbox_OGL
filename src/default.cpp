#include "default.h"

GLFWwindow* InitializeProject(int width, int height, const char* title)
{
    if (!glfwInit())
    {
        throw std::runtime_error("FATAL ERROR: GLFW initialization failed.");
    };
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(width,height,title,nullptr,nullptr);
    if(!window)
    {
        throw std::runtime_error("FATAL ERROR: Window creation failed.");
    };
    glfwMakeContextCurrent(window);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        throw std::runtime_error("FATAL ERROR: Failed to load OPENGL functions.");
    }
    return window;
};

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
    glUseProgram(program);
    
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    return program;


};


// Camera functions 

void FPSCamera::rotatecamera(float yawinc , float pitchinc)
{
    yaw += yawinc;
    pitch += pitchinc;

    if (pitch > 89.0f)
    {
        pitch = 89.0f;
    }
    else if (pitch < -89.0f)
    {
        pitch = -89.0f;
    }
    float yc = std::cos(glm::radians(yaw));
    float ys = std::sin(glm::radians(yaw));

    float pc = std::cos(glm::radians(pitch));
    float ps = std::sin(glm::radians(pitch));

    this->camfront.x = yc * pc ;
    this->camfront.y = ps;
    this->camfront.z = ys * pc ;

    this->camfront = glm::normalize(this->camfront);
};

void FPSCamera::updateViewMatrix()
{
    glm::vec3 right = glm::normalize(glm::cross(this->camfront,this->worldup));
    glm::vec3 camup = glm::normalize(glm::cross(right,this->camfront));

    this->viewmatrix = glm::lookAt(campos, campos+camfront, camup);
};

void FPSCamera::processMouseInput(GLFWwindow* window,bool& firstmouse)
{
    static float lastx = 0 ;
    static float lasty = 0 ;

    double curx ,cury ;

    glfwGetCursorPos(window,&curx,&cury);

    if (firstmouse)
    {
        lastx = curx ;
        lasty = cury ;
        firstmouse = false ;
    }




    float deltax = curx - lastx;
    float deltay = lasty - cury ; 

    lastx  = curx ; 
    lasty = cury ;

    const float sensitivity = 0.1 ;



    this->rotatecamera(deltax*sensitivity,deltay*sensitivity);
    this->updateViewMatrix();

};

void FPSCamera::processkeyboardinput(GLFWwindow* window)
{
    const float speed = 0.1f ;
    

    if (glfwGetKey(window,GLFW_KEY_W) == GLFW_PRESS)
    {
        this->campos += this->camfront*speed;
    }
    if (glfwGetKey(window,GLFW_KEY_S) == GLFW_PRESS)
    {
        this->campos -= this->camfront*speed;
    }
    if (glfwGetKey(window,GLFW_KEY_A) == GLFW_PRESS)
    {
        this->campos -= glm::normalize(glm::cross(camfront,worldup)) * speed;
    }
    if (glfwGetKey(window,GLFW_KEY_D) == GLFW_PRESS)
    {
        this->campos += glm::normalize(glm::cross(camfront,worldup)) * speed;
    }

    this->updateViewMatrix();
};

void gui::EntityCreationWindow(float& grav)
{
    ImGui::SetNextWindowSize (ImVec2(200,300),ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImVec2(0,0),ImGuiCond_FirstUseEver);
    ImGui::Begin("Main",&this->OpenEntityCreationWindow);
    ImGui::SliderFloat("Gravity",&grav,0.0f,20.0f);

    if (ImGui::Button("Create Entity")) 
    {
        this->OpenEntityPropertiesWindow = true;
    };

    if (ImGui::Button("Create Bound")) 
    {
        this->OpenEntityPropertiesWindow = true;
    };


    ImGui::End();
}

void gui::EntityPropertiesWindow(Scene* scene)
{
    
    ImGui::SetNextWindowSize (ImVec2(500,600),ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos (ImVec2(300,0),ImGuiCond_FirstUseEver);

    static float position [3] = {0.0f ,0.0f ,0.0f};
    static float velocity [3] = {0.0f ,0.0f ,0.0f};
    static float acceleration [3] = {0.0f ,0.0f ,0.0f};
    static float mass = 1.0f;
    static bool iscollider {false};

    static float color [3] = {1.0f ,1.0f ,1.0f};
    char name [20];
    int id_param = 0 ;




    ImGui::Begin("Object Parameters",&this->OpenEntityPropertiesWindow);

    ImGui::InputInt("Entity Id",&id_param);
    ImGui::InputText("Entity Name", name,20);

    ImGui::SetNextItemWidth(400);
    ImGui::SliderFloat3("Position",position,-10.0f ,10.0f);

    ImGui::SetNextItemWidth(400);
    ImGui::SliderFloat3("Velocity",velocity,-10.0f,10.0f);

    ImGui::SetNextItemWidth(400);
    ImGui::SliderFloat3("Acceleration",acceleration,-10.0f,10.0f);

    ImGui::SetNextItemWidth(400);
    ImGui::SliderFloat3("Color",color,0.0f ,1.0f);

    ImGui::SetNextItemWidth(400);
    ImGui::SliderFloat("Side",&mass,0.1f,5.0f);

    ImGui::RadioButton("Collider",iscollider);

    if (ImGui::Button("Create"))
    {
        // Creating Objects
        Entity* ent  = scene->newEntity(id_param);

        if (!ent)
        {
            std::cout << "Entity Creation Failed" << std::endl;
            return ;
        }
        ent->col  = {color[0],color[1],color[2]};

        // Create mesh for entity
        ent->entitymesh = new mesh();
        ent->entitymesh->gencuboidmesh({mass, mass,mass});
        ent->entitybody = new rigidbody(shapetype::cube,{mass,mass,mass});
        ent->entitybody->isCollider = true ;
        ent->entitybody->velocity = {velocity[0],velocity[1],velocity[2]};
        ent->entitybody->acceleration = {acceleration[0],acceleration[1],acceleration[2]};
        ent->entitybody->hcubeside = {mass, mass,mass};
        ent->entitybody->isCollider = iscollider;
        ent->updateModelMatrix();

    }

    if (ImGui::Button("Done"))
    {
        this->OpenEntityPropertiesWindow = false ;
    }

    ImGui::End();
}
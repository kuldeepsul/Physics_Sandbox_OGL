#include "ogldef.h"

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

///////////////////////////////////////////////////////////////////
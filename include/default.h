#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <string>
#include <iostream>
#include <filesystem>
#include <sstream>
#include <fstream>
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>
#include "shapes.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

GLFWwindow* InitializeProject(int width, int height, const char* title);
std::string readShaderFile(std::string path);
void checkShaderCompilation(unsigned int &shader);
unsigned int getShaderProgram(std::string& path_vert,std::string path_frag);

enum  class  cursormode {gui_mode, camera_mode};


class FPSCamera
{
    public:
    glm::vec3 campos;
    glm::vec3 camfront;
    glm::vec3 worldup {0.0f , 1.0f , 0.0f};
    glm::mat4 viewmatrix;

    float yaw = -90.0f ;
    float pitch = 0.0f;

    void rotatecamera(float yawinc, float pitchinc);  
    void processMouseInput(GLFWwindow* window,bool& firstmouse);
    void processkeyboardinput(GLFWwindow* window);
    void updateViewMatrix ();
};

class gui
{
    public:
    bool OpenEntityCreationWindow {true};
    bool OpenEntityPropertiesWindow {false};
    bool OpenEntityUpdateWindow {false};
    bool OpenContactDebugWindow{true};
    bool OpenSATDebugWindow {true};


    void show(Scene* scene);

    void EntityCreationWindow(Scene* scene);
    void EntityPropertiesWindow(Scene* scene);
    void EntityUpdateWindow(Scene* scene);
    void ContactDebugWindow(Scene* scene);
    void SATDebugWindow(Scene* scene);
    
    

    void GetEntityProp(Scene* scene,int &id_param);
    void ShowContactProperties(Scene* scene,int index);
    void ShowSATaxes(Scene* scene,int index);
    
};


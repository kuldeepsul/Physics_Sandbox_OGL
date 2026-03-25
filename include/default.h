#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <iostream>
#include <filesystem>
#include <sstream>
#include <fstream>
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>

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


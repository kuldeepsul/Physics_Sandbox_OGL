#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>

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
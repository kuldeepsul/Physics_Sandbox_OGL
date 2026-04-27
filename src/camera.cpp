#include "camera.h"

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


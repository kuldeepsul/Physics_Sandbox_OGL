#pragma once
#include "shapes.h"
#include "camera.h"

// Functions to setup OpenGL Boilerplate

GLFWwindow* InitializeProject(int width, int height, const char* title);
std::string readShaderFile(std::string path);
void checkShaderCompilation(unsigned int &shader);
unsigned int getShaderProgram(std::string& path_vert,std::string path_frag);

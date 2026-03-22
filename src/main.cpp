#include "default.h"
#include "shapes.h"
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>

int main()
{
    const char title [] = {"Trial"};   
    GLFWwindow* window;

    try
    {
        window = InitializeProject(1920,1080,title);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    glViewport(0,0,1920,1080);
    
    glm::vec3 cubePositions[] = {
        glm::vec3( 0.0f,  0.0f,  0.0f), 
        glm::vec3( 0.0f,  50.0f, -15.0f), 
        glm::vec3(-1.5f,  20.2f, -2.5f),  
        glm::vec3(-3.8f,  20.0f, -12.3f),  
        glm::vec3( 2.4f,  20.4f, -3.5f),  
        glm::vec3(-1.7f,  30.0f, -7.5f), 
        glm::vec3(-2.7f,  30.0f, -5.5f)   
    };

    ///////////////////////////////////////////////
    // Initializing Camera
    FPSCamera cam;
    cam.campos = {0.0f, 10.0f, 0.0f};
    cam.camfront = {0.0f, 0.0f ,3.0f};
    cam.updateViewMatrix();
    ///////////////////////////////////////////////

    ////////////////////////////////////////////////
    // Scene Generation.
    MeshGenerator MeshGen;
    float cubeside = 2.0f;
    glm::vec3 cubesides = {cubeside,cubeside,cubeside};

    mesh mcube = MeshGen.gencuboidmesh(cubesides);
    mesh mplane = MeshGen.genplanemesh(100.0f , 100.0f);
    mesh mobject = MeshGen.readobj("objs/sphere.obj");

    
    rigidbody* rcube1 = new rigidbody(shapetype::cube ,cubesides);
    rigidbody* rcube2 = new rigidbody(shapetype::cube ,cubesides);
    rigidbody* rcube3 = new rigidbody(shapetype::cube ,cubesides);
    rigidbody* rcube4 = new rigidbody(shapetype::cube ,cubesides);
    rigidbody* rcube5 = new rigidbody(shapetype::cube ,cubesides);
    rigidbody* rplane = new rigidbody(shapetype::plane ,{100.0f,100.0f},{0.0f ,1.0f ,0.0f});

    Scene s1;
    s1.newEntity(1,&mobject,rcube1);
    s1.newEntity(2,&mobject,rcube2);
    s1.newEntity(3,&mobject,rcube3);
    s1.newEntity(4,&mobject,rcube4);
    s1.newEntity(5,&mobject,rcube5);
    s1.newEntity(6,&mplane,rplane);

    int i {0};
    for (Entity* ent : s1.entities)
    {
        
        i++;
        if(ent->id != 6)
        {     
            ent->entitybody->position = cubePositions[i] ;  
            ent->scaleEntity({3.0f,3.0f,3.0f});
            ent->entitybody->velocity = {0.0f,0.0f,0.0f};
            ent->col = {0.4f,0.3f,0.8f};
            ent->updateModelMatrix();
                       

        }
        else
        {
            ent->entitybody->velocity = {0.0f,0.0f,0.0f};
            ent->entitybody->position = {500.0f,0.0f,500.0f};
            ent->col = {0.1f,0.3f,0.5f};
        }
    
    }

    ///////////////////////////////////////////////////////
    
    // Shaders
    std::string path_vert = "shader/Vertex_shader.glsl";
    std::string path_frag = "shader/Lighting_shader.glsl";

    unsigned int program = getShaderProgram(path_vert,path_frag);

    // Perspective 
    glm::mat4 Presp = glm::perspective(glm::radians(90.0f), (1920.0f/1080.0f),0.1f,100.0f);
    unsigned int persloc = glGetUniformLocation(program,"Perspective_mat");
    glUniformMatrix4fv(persloc,1,GL_FALSE,glm::value_ptr(Presp));
    

    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window))
    {
        
        if(glfwGetKey(window,GLFW_KEY_ESCAPE)==GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window,GLFW_TRUE);
        }

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

        // Camera Controls
        glfwSetInputMode(window,GLFW_CURSOR,GLFW_CURSOR_DISABLED);
        cam.processkeyboardinput( window);
        cam.processMouseInput(window);


        // View Matrix Based on  Camera Controls
        unsigned int viewloc = glGetUniformLocation(program,"View_mat");
        glUniformMatrix4fv(viewloc,1,GL_FALSE,glm::value_ptr(cam.viewmatrix));

        // Camera as lightsource
        unsigned int lightposloc = glGetUniformLocation(program,"lightpos");
        glUniform3fv(lightposloc,1,glm::value_ptr(cam.campos));


        for (Entity* ent : s1.entities)
        {

            if (ent->id != 6)
            {
                ent->entitybody->acceleration = 9.8f * glm::vec3 (0.0f , -1.0f ,0.0f);
                ent->entitybody->velocity += (0.01f) * ent->entitybody->acceleration;
                ent->entitybody->position += (0.01f) * ent->entitybody->velocity;
                //ent->updateModelMatrix();

                if (ent->entitybody->position.y - ent->entitybody->hcubeside.y <= 0)
                {
                    ent->entitybody->velocity.y = -ent->entitybody->velocity.y ;
                }
            }
        }

        for (Entity* ent : s1.entities)
        {
            // Passing Entity color as a uniform.
            unsigned int objcolloc = glGetUniformLocation(program,"objcol");
            glUniform3fv(objcolloc,1,glm::value_ptr(glm::normalize(ent->col)));

            // Passing Entity Model Matrix as a uniform. 
            unsigned int modelloc = glGetUniformLocation(program,"Model_mat");
            glUniformMatrix4fv(modelloc,1,GL_FALSE,glm::value_ptr(ent->model_matrix));

            glBindVertexArray(ent->entitymesh->VAO);
            glDrawArrays(GL_TRIANGLES,0,ent->entitymesh->vertexcount);
            
        }


        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    
}
#include <iostream>
#include "default.h"
#include "shapes.h"
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

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

    ///////////////////////////////////
    // Initialize Imgui .
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // Setup Rendering Backend .
    ImGui_ImplGlfw_InitForOpenGL(window,true);
    ImGui_ImplOpenGL3_Init();

    ///////////////////////////////////////////////
    // Initializing Camera
    FPSCamera cam;
    cam.campos = {0.0f, 10.0f, 0.0f};
    cam.camfront = {0.0f, 0.0f ,3.0f};
    cam.updateViewMatrix();
    ///////////////////////////////////////////////
    std::string path_vert_flat =  "shader/Vertex_shader_flat.glsl";
    std::string path_vert = "shader/Vertex_shader.glsl";
    std::string path_frag_lighting = "shader/Lighting_shader.glsl";
    std::string path_frag_debug = "shader/debug_shader.glsl";
    std::string path_frag_flat = "shader/flat_shader.glsl";

    Scene s1;
    s1.shaderprogram = getShaderProgram(path_vert,path_frag_lighting);
    Scene Utils;
    //Utils.showgrids(4.0f);
    Utils.shaderprogram = getShaderProgram(path_vert_flat,path_frag_flat);

    //////////////////////////////////////////////////////
    // Manual Setup

    // Bounding Box.
    Entity* ent  = s1.newEntity(0);
    glm::vec3 side = {20.0f ,10.0f , 20.0f};
    // Create mesh for entity
    ent->name = "Bounding Box";
    ent->id  = 0;

    ent->entitymesh = new mesh();
    ent->entitymesh->gencuboidmesh(side);

    ent->entitybody = new rigidbody(shapetype::cube,side);
    ent->entitybody->mass = side.x;

    ent->isWireFrame = true;
    ent->entitybody->isCollider = false;
    ent->entitybody->position = {0.0f,5.0f,0.0f};
    ent->updateModelMatrix();
    s1.scene_bound = ent;

    // create Cube 01

    Entity* ent1  = s1.newEntity(1);
    glm::vec3 side1 = {2.0f ,2.0f , 2.0f};
    // Create mesh for entity
    ent1->name = "Cube 01";
    ent1->id  = 1;

    ent1->entitymesh = new mesh();
    ent1->entitymesh->gencuboidmesh(side1);

    ent1->entitybody = new rigidbody(shapetype::cube,side1);
    ent1->entitybody->mass = side1.x;

    ent1->isWireFrame = false;
    ent1->entitybody->isCollider = true;
    ent1->entitybody->position = {-3.0f,3.0f,0.0f};
    ent1->entitybody->createcontactbodies(&Utils);

    // create Cube 02
    
    Entity* ent2  = s1.newEntity(2);
    glm::vec3 side2 = {1.0f ,1.0f , 1.0f};
    // Create mesh for entity
    ent2->name = "Cube 02";
    ent2->id  = 2;

    ent2->entitymesh = new mesh();
    ent2->entitymesh->gencuboidmesh(side2);

    ent2->entitybody = new rigidbody(shapetype::cube,side2);
    ent2->entitybody->mass = side2.x;

    ent2->isWireFrame = false;
    ent2->entitybody->isCollider = true;
    ent2->entitybody->position = {2.0f, 3.0f,0.0f};
 
    /////////////////////////////////////////////////////////////////


    glEnable(GL_DEPTH_TEST);
    bool isPaused {false} ;
    bool showcontrols {true};
    bool showobjectproperties {false};
    static float grav {0.0f};
    cursormode currentmode = cursormode::camera_mode;
    static bool firstmouse = true ;
    bool drawcontact = true;


    gui* ui = new gui();

    // Perspective Matrix.
    glm::mat4 Presp = glm::perspective(glm::radians(90.0f), (1920.0f/1080.0f),0.1f,100.0f);


    while (!glfwWindowShouldClose(window))
    {
        
        if(glfwGetKey(window,GLFW_KEY_ESCAPE)==GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window,GLFW_TRUE);
        }

        if(glfwGetKey(window,GLFW_KEY_SPACE)==GLFW_PRESS)
        {
            if(isPaused)
            {
                isPaused = false;
            }
            else
            {
                isPaused =true ;
            }
        }

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        if (glfwGetKey(window,GLFW_KEY_M) == GLFW_PRESS)
        {
            if (currentmode == cursormode::camera_mode) 
            {
                currentmode = cursormode::gui_mode;
                firstmouse = true ;
                isPaused = true;
            }
            else  
            {
                currentmode = cursormode::camera_mode;
                isPaused = false ;
            }
        }

        // Camera Controls
        if (currentmode == cursormode::camera_mode)
        {
            glfwSetInputMode(window,GLFW_CURSOR,GLFW_CURSOR_DISABLED);
            cam.processkeyboardinput( window);
            cam.processMouseInput(window,firstmouse);
        }
        else if (currentmode == cursormode::gui_mode)
        {
            glfwSetInputMode(window,GLFW_CURSOR,GLFW_CURSOR_NORMAL);
        }
        

        // Physics Loop
        if(!isPaused)
        {
            for (Entity* ent : s1.entities)
            {

                if (ent->entitybody->isCollider)
                {
                    ent->entitybody->acceleration = grav * glm::vec3 (0.0f , -1.0f ,0.0f);
                    ent->entitybody->velocity += (0.01f) * ent->entitybody->acceleration;
                    ent->entitybody->position += (0.01f) * ent->entitybody->velocity;
                    ent->updateModelMatrix();
                    ent->entitybody->checkboundcollision(s1.scene_bound->entitybody);

                    for (Entity* ent2 : s1.entities)
                    {
                        if(ent2->entitybody->isCollider)
                        {
                            if(ent2 == ent)
                            {
                                continue;
                            }
                            else
                            {
                                ent->entitybody->checkAABB(ent2->entitybody);
                            }
                        }
                    }
                }
            }
        }   

        // Collision Detection.
        for (Entity* ent1 : s1.entities)
        {
            
            bool collision_status = false;

            if (ent1->entitybody->isCollider)
            {
                for (Entity* ent2 : s1.entities)
                {
                    
                    if (ent2->entitybody->isCollider)
                    {
                        if (ent1->id == ent2->id)
                        {
                            continue;
                        }
                        else
                        {
                            collision_status = ent1->entitybody->checkSAT(ent2->entitybody);

                        }
                        if(collision_status)
                        {
                            ent1->col = {1.0f,0.0f,0.0f};
                            ent2->col = {1.0f,0.0f,0.0f};
                            if(ent->entitybody->bodycontact)
                            {
                                ent->entitybody->updatecontact();
                            }
                            
                        }
                        else
                        {
                            ent1->col = {1.0f,1.0f,1.0f};
                            ent2->col = {1.0f,1.0f,1.0f};
                            if(ent->entitybody->bodycontact)
                            {
                                ent->entitybody->contactbody->position = ent->entitybody->position;
                            }
                        }
                    }
                }
            }
        }

        for (Entity* ent : s1.entities)
        {
            glUseProgram(s1.shaderprogram);
            // Perspective 
            unsigned int persloc = glGetUniformLocation(s1.shaderprogram,"Perspective_mat");
            glUniformMatrix4fv(persloc,1,GL_FALSE,glm::value_ptr(Presp));

            // View Matrix Based on  Camera Controls
            unsigned int viewloc = glGetUniformLocation(s1.shaderprogram,"View_mat");
            glUniformMatrix4fv(viewloc,1,GL_FALSE,glm::value_ptr(cam.viewmatrix));

            // Camera as lightsource
            unsigned int lightposloc = glGetUniformLocation(s1.shaderprogram,"lightpos");
            glUniform3fv(lightposloc,1,glm::value_ptr(cam.campos));

            // Passing Entity color as a uniform.
            unsigned int objcolloc = glGetUniformLocation(s1.shaderprogram,"objcol");
            glUniform3fv(objcolloc,1,glm::value_ptr(glm::normalize(ent->col)));

            // Passing Entity Model Matrix as a uniform. 
            unsigned int modelloc = glGetUniformLocation(s1.shaderprogram,"Model_mat");
            glUniformMatrix4fv(modelloc,1,GL_FALSE,glm::value_ptr(ent->model_matrix));


            if (!ent->isWireFrame)
            {
                glBindVertexArray(ent->entitymesh->VAO);
                glDrawArrays(GL_TRIANGLES,0,ent->entitymesh->vertexcount);

        
            }
            else
            {
                glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
                glBindVertexArray(ent->entitymesh->VAO);
                glDrawArrays(GL_TRIANGLES,0,ent->entitymesh->vertexcount);
                glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

            }

            
        }

        for (Entity* ent : Utils.entities)
        {
            glUseProgram(Utils.shaderprogram);
            // Perspective 
            unsigned int persloc = glGetUniformLocation(Utils.shaderprogram,"Perspective_mat");
            glUniformMatrix4fv(persloc,1,GL_FALSE,glm::value_ptr(Presp));

            // View Matrix Based on  Camera Controls
            unsigned int viewloc = glGetUniformLocation(Utils.shaderprogram,"View_mat");
            glUniformMatrix4fv(viewloc,1,GL_FALSE,glm::value_ptr(cam.viewmatrix));

            // Passing Entity color as a uniform.
            unsigned int objcolloc = glGetUniformLocation(Utils.shaderprogram,"objcol");
            glUniform3fv(objcolloc,1,glm::value_ptr(glm::normalize(ent->col)));

            // Passing Entity Model Matrix as a uniform. 
            unsigned int modelloc = glGetUniformLocation(Utils.shaderprogram,"Model_mat");
            glUniformMatrix4fv(modelloc,1,GL_FALSE,glm::value_ptr(ent->model_matrix));

            if (ent->isWireFrame)
            {
                glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
                glBindVertexArray(ent->entitymesh->VAO);
                glDrawArrays(GL_LINES,0,ent->entitymesh->vertexcount);
                glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
            }
            else
            {
                glBindVertexArray(ent->entitymesh->VAO);
                glDrawArrays(GL_LINES,0,ent->entitymesh->vertexcount);
            }

        }

        glfwPollEvents();

        // Start ImGui Frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if(ui->OpenEntityCreationWindow)
        {
            ui->EntityCreationWindow(grav,&s1);
        }
        if (ui->OpenEntityPropertiesWindow)
        {
            ui->EntityPropertiesWindow(&s1);
        }
        if(ui->OpenEntityUpdateWindow)
        {
            ui->EntityUpdateWindow(&s1);
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);

    }
    // Terminate Imgui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
    
}


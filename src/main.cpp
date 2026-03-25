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

    /////////////////////////////////
    
    glm::vec3 cubePositions[] = {
        glm::vec3( 0.0f,  0.0f,  0.0f), 
        glm::vec3( 0.0f,  3.0f, -1.0f), 
        glm::vec3(-1.5f,  2.0f, -2.0f),  
        glm::vec3(-3.8f,  1.0f, -3.0f),  
        glm::vec3( 2.4f,  5.0f, -4.0f),  
        glm::vec3(-1.7f,  3.2f, -5.0f), 
        glm::vec3(-2.7f,  6.0f, -6.0f)   
    };

    glm::vec3 cubeVelocities[] = {
        glm::vec3( 1.0f,  0.0f,  0.0f), 
        glm::vec3( 0.0f,  5.0f,  0.0f), 
        glm::vec3(-6.0f,  4.0f,  1.0f),  
        glm::vec3(-2.0f,  5.0f,  2.0f),  
        glm::vec3( 4.0f,  2.0f,  -3.0f),  
        glm::vec3( -5.0f, -4.0f,  -4.0f), 
        glm::vec3( 1.0f, -6.0f,  5.0f)   
    };
    /*
        glm::vec3 cubePositions[] = {
        glm::vec3( 0.0f,  0.0f,  0.0f), 
        glm::vec3( 0.0f,  3.0f,  0.0f), 
        glm::vec3(-1.5f,  3.0f,  0.0f),  
        glm::vec3(-3.8f,  3.0f,  0.0f),  
        glm::vec3( 2.4f,  3.0f,  0.0f),  
        glm::vec3(-1.7f,  3.2f,  0.0f), 
        glm::vec3(-2.7f,  3.0f,  0.0f)   
    };

    glm::vec3 cubeVelocities[] = {
        glm::vec3( 0.0f,  0.0f,  0.0f), 
        glm::vec3(-2.0f,  0.0f,  0.0f), 
        glm::vec3(-6.0f,  0.0f,  0.0f),  
        glm::vec3(-2.0f,  0.0f,  0.0f),  
        glm::vec3( 2.0f,  0.0f,  0.0f),  
        glm::vec3( 5.0f,  0.0f,  0.0f), 
        glm::vec3( 8.0f,  0.0f,  0.0f) 
    };
    */

    ///////////////////////////////////////////////
    // Initializing Camera
    FPSCamera cam;
    cam.campos = {0.0f, 10.0f, 0.0f};
    cam.camfront = {0.0f, 0.0f ,3.0f};
    cam.updateViewMatrix();
    ///////////////////////////////////////////////
    // Creating bounds 
    bound domain (10.0f,-10.0f,10.0f,0.0f,10.0f,-10.0f);

    ////////////////////////////////////////////////
    // Scene Generation.
    MeshGenerator MeshGen;
    float cubeside = 1.0f;
    glm::vec3 cubesides = {cubeside,cubeside,cubeside};

    mesh mcube1 = MeshGen.gencuboidmesh(cubesides);
    mesh mcube2 = MeshGen.gencuboidmesh(cubesides);
    mesh mcube3 = MeshGen.gencuboidmesh(cubesides);
    mesh mcube4 = MeshGen.gencuboidmesh(cubesides);
    mesh mcube5 = MeshGen.gencuboidmesh(cubesides);
    mesh mcube6 = MeshGen.gencuboidmesh(cubesides);

    mesh mplane = MeshGen.genplanemesh(100.0f , 100.0f);
    mesh mobject = MeshGen.readobj("objs/sphere.obj");
    mesh mbound = MeshGen.gencuboidmesh({20.0f,10.0f,20.0f});

    
    rigidbody* rcube1 = new rigidbody(shapetype::cube ,cubesides);
    rigidbody* rcube2 = new rigidbody(shapetype::cube ,cubesides);
    rigidbody* rcube3 = new rigidbody(shapetype::cube ,cubesides);
    rigidbody* rcube4 = new rigidbody(shapetype::cube ,cubesides);
    rigidbody* rcube5 = new rigidbody(shapetype::cube ,cubesides);
    rigidbody* rcube6 = new rigidbody(shapetype::cube ,cubesides);

    rigidbody* rbound = new rigidbody(shapetype::cube ,{10.0f,5.0f,10.0f});

    Scene s1;
    s1.newEntity(1,&mcube1,rcube1);
    s1.newEntity(2,&mcube2,rcube2);
    s1.newEntity(3,&mcube3,rcube3);
    s1.newEntity(4,&mcube4,rcube4);
    s1.newEntity(5,&mcube5,rcube5);
    s1.newEntity(7,&mcube6,rcube6);


    s1.newEntity(6,&mbound,rbound);
    

    int i {0};
    for (Entity* ent : s1.entities)
    {  
        i++;

        ent->entitybody->position = cubePositions[i] ;  
        ent->scaleEntity({1.0f,1.0f,1.0f});
        ent->entitybody->velocity = 1.5f * cubeVelocities[i];
        ent->col = {0.4f,0.3f,0.8f};
        ent->updateModelMatrix(); 

        if (ent->id == 6 )
        {
            ent->entitybody->position = {0.0f , 5.0f ,0.0f};
            ent->col = {1.0f,1.0f,1.0f};
            ent->entitybody->isCollider = false;
            ent->updateModelMatrix();
            ent->isWireFrame = true ;
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
    bool isPaused {false} ;
    bool showcontrols {true};
    static float grav {0.0f};
    cursormode currentmode = cursormode::camera_mode;
    static bool firstmouse = true ;


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
            {currentmode = cursormode::gui_mode;
            firstmouse = true ;}
            else  {currentmode = cursormode::camera_mode;}
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
        



        // View Matrix Based on  Camera Controls
        unsigned int viewloc = glGetUniformLocation(program,"View_mat");
        glUniformMatrix4fv(viewloc,1,GL_FALSE,glm::value_ptr(cam.viewmatrix));

        // Camera as lightsource
        unsigned int lightposloc = glGetUniformLocation(program,"lightpos");
        glUniform3fv(lightposloc,1,glm::value_ptr(cam.campos));

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
                    ent->entitybody->checkboundcollision(domain);

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

        for (Entity* ent : s1.entities)
        {
            // Passing Entity color as a uniform.
            unsigned int objcolloc = glGetUniformLocation(program,"objcol");
            glUniform3fv(objcolloc,1,glm::value_ptr(glm::normalize(ent->col)));

            // Passing Entity Model Matrix as a uniform. 
            unsigned int modelloc = glGetUniformLocation(program,"Model_mat");
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

        glfwPollEvents();

        // Start ImGui Frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if(showcontrols)
        {
            ImGui::Begin("Object Controls");
            ImGui::SliderFloat("Gravity",&grav,0.0f,20.0f);
            if (ImGui::Button("Create Box")) 
            {
                
            };


            ImGui::End();

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
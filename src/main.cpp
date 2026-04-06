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

    ent->entitymesh = new Mesh();
    ent->entitymesh->gencuboidmesh(side);

    ent->entitybody = new RigidBody(ShapeType::cube,side);
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

    ent1->entitymesh = new Mesh();
    ent1->entitymesh->gencuboidmesh(side1);

    ent1->entitybody = new RigidBody(ShapeType::cube,side1);
    ent1->entitybody->mass = side1.x;

    ent1->isWireFrame = false;
    ent1->entitybody->isCollider = true;
    ent1->entitybody->position = {-3.0f,3.0f,0.0f};

    // create Cube 02
    
    Entity* ent2  = s1.newEntity(2);
    glm::vec3 side2 = {1.0f ,1.0f , 1.0f};
    // Create mesh for entity
    ent2->name = "Cube 02";
    ent2->id  = 2;

    ent2->entitymesh = new Mesh();
    ent2->entitymesh->gencuboidmesh(side2);

    ent2->entitybody = new RigidBody(ShapeType::cube,side2);
    ent2->entitybody->mass = side2.x;

    ent2->isWireFrame = false;
    ent2->entitybody->isCollider = true;
    ent2->entitybody->position = {2.0f, 3.0f,0.0f};
 
    /////////////////////////////////////////////////////////////////

    static float grav {0.0f};
    glEnable(GL_DEPTH_TEST);
    bool isPaused {false} ;
    bool showcontrols {true};
    bool showobjectproperties {false};
    static bool firstmouse {true} ;
    bool drawcontact {true};

    cursormode currentmode = cursormode::camera_mode;



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
                    CollisionFunc::checkboundcollision(ent->entitybody,s1.scene_bound->entitybody);

                    for (Entity* ent2 : s1.entities)
                    {
                        if(ent2->entitybody->isCollider)
                        {
                            ent2->entitybody->resolvecontacts();
                            if(ent2 == ent)
                            {
                                continue;
                            }
                            else
                            {
                                CollisionFunc::checkAABB(ent->entitybody,ent2->entitybody);
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
                            collision_status = CollisionFunc::checkSAT(ent1->entitybody , ent2->entitybody);
                        }
                        if(collision_status)
                        {
                            ent1->col = {1.0f,0.0f,0.0f};
                            ent2->col = {1.0f,0.0f,0.0f};
                            
                        }
                        else
                        {
                            ent1->col = {1.0f,1.0f,1.0f};
                            ent2->col = {1.0f,1.0f,1.0f};

                        }
                    }
                }
            }
        }

        // Render Scenes.
        s1.drawScene(Presp,cam.viewmatrix,cam.campos);
        Utils.drawScene(Presp,cam.viewmatrix,cam.campos);
        

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


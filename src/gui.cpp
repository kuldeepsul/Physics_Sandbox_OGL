#include "gui.h"

//////////////////////////////////////////////////////////////
void gui::show(Scene* s1)
{
    if(this->OpenEntityCreationWindow)
    {
        this->EntityCreationWindow(s1);
    }
    if (this->OpenEntityPropertiesWindow)
    {
        this->EntityPropertiesWindow(s1);
    }
    if(this->OpenEntityUpdateWindow)
    {
        this->EntityUpdateWindow(s1);
    }
    if (this->OpenContactDebugWindow)
    {
        this->ContactDebugWindow(s1);
    }
    if (this->OpenSATDebugWindow)
    {
        this->SATDebugWindow(s1);
    }
};

void gui::EntityCreationWindow(Scene* scene)
{
    ImGui::SetNextWindowSize (ImVec2(200,300),ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImVec2(0,0),ImGuiCond_FirstUseEver);
    ImGui::Begin("Main",&this->OpenEntityCreationWindow);
    //ImGui::SliderFloat("Gravity",&grav,0.0f,20.0f);

    if (ImGui::Button("Create Entity")) 
    {
        this->OpenEntityPropertiesWindow = true;

    };

    if (ImGui::Button("Update Entity")) 
    {
        this->OpenEntityUpdateWindow = true;
    };


    ImGui::End();
}

void gui::EntityPropertiesWindow(Scene* scene)
{
    
    ImGui::SetNextWindowSize (ImVec2(500,600),ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos (ImVec2(300,0),ImGuiCond_FirstUseEver);

    static float side  = 1.0f;
    static char name_param [20];
    static int id_param = 0 ;
    static bool isbound = false;




    ImGui::Begin("Object Parameters",&this->OpenEntityPropertiesWindow);

    ImGui::InputInt("Entity Id",&id_param);
    ImGui::InputText("Entity Name", name_param,20);
    ImGui::SliderFloat("Size/Mass",&side,0.2f,20.0f,"%.1f");

    ImGui::Checkbox("Set as Bounding Box",&isbound);


    if (ImGui::Button("Create"))
    {
        // Creating Objects
        Entity* ent  = scene->newEntity(id_param);

        if (!ent)
        {
            std::cout << "Entity Creation Failed" << std::endl;
            return ;
        }

        // Create mesh for entity
        ent->name = name_param;
        ent->id  = id_param;

        ent->entitymesh = new Mesh();
        ent->entitymesh->gencuboidmesh({side, side, side});

        ent->entitybody = new RigidBody(ShapeType::cube,{side, side, side});
        ent->entitybody->mass = side;

        if (isbound)
        {
            ent->isWireFrame = true;
            ent->entitybody->isCollider = false;
            scene->scene_bound = ent;
        }
        
        ent->updateModelMatrix();

    }

    if (ImGui::Button("Close"))
    {
        this->OpenEntityPropertiesWindow = false ;
    }

    ImGui::End();
}

void gui::GetEntityProp(Scene* scene ,int &id_param)
{
    static int last_id {-1};
    static bool id_changed = false ;

    if (last_id != id_param)
    {
        id_changed = true;
        last_id = id_param;
    }
    Entity* ent = scene->entities[id_param];

    static glm::vec3 pos  = ent->entitybody->position;
    static glm::vec3 vel = ent->entitybody->velocity;
    static glm::vec3 axis = {1.0f ,0.0f ,0.0f};
    static float angle = 0.0f;

    static glm::vec3 color = ent->col;
    static bool enable_collision = ent->entitybody->isCollider ;
    static bool enable_wireframe = ent->isWireFrame;

    if (id_changed)
    {
        pos = ent->entitybody->position;
        vel = ent->entitybody->velocity;
        color = ent->col;
        enable_collision = ent->entitybody->isCollider;
        enable_wireframe = ent->isWireFrame;
        id_changed = false;
    }
    else
    {
        pos = ent->entitybody->position;
        vel = ent->entitybody->velocity;
        color = ent->col;
        enable_collision = ent->entitybody->isCollider;
        enable_wireframe = ent->isWireFrame;
        id_changed = false;
    }


    ImGui::Checkbox("Enable Collisions" , &enable_collision);
    ImGui::Checkbox("Enable Wireframe" , &enable_wireframe);
    
    ImGui::SliderFloat3("Position",&pos.x,-5.0f ,5.0f,"%.1f");
    ImGui::SliderFloat3("Velocity",&vel.x,-5.0f ,5.0f,"%.1f");

    ImGui::SliderFloat3("Orientation Axis" , &axis.x,-1.0f,1.0f,"%.1f");
    ImGui::SliderFloat("Angle",&angle,-180.0f,180.0f,"%.1f");


    ImGui::ColorPicker3("Object Color" , &color.x);



    // assign Properties to object
    
    ent->entitybody->isCollider = enable_collision;
    ent->col = {color[0],color[1],color[2]};
    ent->entitybody->position = {pos[0],pos[1],pos[2]};
    ent->entitybody->velocity = {vel[0],vel[1],vel[2]};
    ent->entitybody->isCollider = enable_collision;
    ent->entitybody->updateorientation(angle,axis);

    static glm::vec3 ent1_old_color;
    static glm::vec3 ent2_old_color;

    static bool collision_status = false;

    ent->updateModelMatrix();

}

void gui::EntityUpdateWindow(Scene* scene)
{
    ImGui::Begin("Properties Editor",&this->OpenEntityUpdateWindow);
    // Parent content
    ImGui::Text("Top panel");

    // Child region
    ImGui::BeginChild("EntityList", ImVec2(0, 300), true);

    static int selectedEntity {-1};
    for (int i {0} ; i < scene->entities.size() ; i++)
    {
        bool selected  = (selectedEntity == i);


        if (ImGui::Selectable(scene->entities[i]->name.c_str() , selected))
        {
            selectedEntity = i ;
        }
        
    }
    ImGui::EndChild();

    static bool OpenEditPanel = false;
    if (ImGui::Button("Edit"))
    {
        OpenEditPanel = true;
    }
    if (OpenEditPanel)
    {
        if (selectedEntity != -1)
        {
            ImGui::BeginChild("Props",ImVec2(400,400));

            GetEntityProp(scene,selectedEntity);

            ImGui::EndChild();
        }
    }

    
    ImGui::End();
    if (!this->OpenEntityUpdateWindow)
    {
        selectedEntity = -1;
        OpenEditPanel = false;
    }
}

void gui::ShowContactProperties(Scene* scene,int index)
{
    glm::vec3 norm = scene->contacts[index]->normal;
    glm::vec3 point = scene->contacts[index]->point;
    ImGui::Text("Contact Normal.");
    ImGui::Text("X : %.2f , Y : %.2f , Z : %.2f ",norm.x,norm.y,norm.z);
    ImGui::Text("Contact Point.");
    ImGui::Text("X : %.2f , Y : %.2f , Z : %.2f ",point.x,point.y,point.z);
}

void gui::ShowSATaxes(Scene* scene,int index)
{
    glm::vec3 axis = scene->SATaxes[index];

    ImGui::Text("X : %.2f , Y : %.2f , Z : %.2f ",axis.x,axis.y,axis.z);
}

void gui::ContactDebugWindow(Scene* scene)
{
    ImGui::Begin(" Contact Debug Window");

    ImGui::BeginChild("Contact Data");
    for(int i {0} ; i < scene->contacts.size() ; i++)
    {
        
        ImGui::Text("Contact %i", i);

        ShowContactProperties(scene,i);


    }
    ImGui::EndChild();
    ImGui::End();
}

void gui::SATDebugWindow(Scene* scene)
{
    
    ImGui::Begin(" Current SAT Axes.");

    ImGui::BeginChild("SAT Axes");
    for (int i {0} ; i < scene->SATaxes.size() ; i++)
    {
        
        ImGui::Text("Axis %i" , i );
        ShowSATaxes(scene,i);

    }
    ImGui::EndChild();
    ImGui::End();
}

////////////////////////////////////////////////////////////
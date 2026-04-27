#pragma once
#include "shapes.h"
#include "camera.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

enum  class  cursormode {gui_mode, camera_mode};

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
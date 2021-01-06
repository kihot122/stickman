#pragma once
#include <GLFW/glfw3.h>
#include "../render/Renderer.hpp"

class Keyboard{
    Renderer app;
    
    public:
    Keyboard(Renderer app_);
    void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    void init();

};




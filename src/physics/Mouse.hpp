#pragma once
#include <GLFW/glfw3.h>
#include "../render/Renderer.hpp"

class Mouse{
    bool isMousePressed;
    Renderer app;

    public:
    Mouse(Renderer app_);
    static void cursorPositionCallback(GLFWwindow* window, double xPos, double yPos);
    void cursorEnterCallback(GLFWwindow* window, int entered);
    void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    void init();
    bool getIsMousePressed();
};
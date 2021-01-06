#include "Mouse.hpp"

Mouse::Mouse(Renderer app_){
    isMousePressed = false;
    app = app_;
    init();
}

void Mouse::init(){
    glfwSetCursorPosCallback(app.GetWindowHandle(), cursorPositionCallback);
    glfwSetInputMode(app.GetWindowHandle(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSetCursorEnterCallback(app.GetWindowHandle(), cursorEnterCallback);
    glfwSetMouseButtonCallback(app.GetWindowHandle(), mouseButtonCallback);
}
static void Mouse::cursorPositionCallback(GLFWwindow* window, double xPos, double yPos){

}
void Mouse::cursorEnterCallback(GLFWwindow* window, int entered){
    if(entered){
        std::cout << "Entered Window" << std::endl;
    }
    else std::cout << "Left Window" << std::endl;
}
void Mouse::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods){

}
bool Mouse::getIsMousePressed(){return isMousePressed;}
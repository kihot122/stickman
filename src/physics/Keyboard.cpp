#include "Keyboard.hpp"

Keyboard::Keyboard(Renderer app_){
    app = app_;
    init();
}

void Keyboard::init(){
    glfwSetKeyCallback(app.GetWindowHandle(), keyCallback);

}
void Keyboard::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods){
    std::cout << key << std::endl;
}
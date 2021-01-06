#include <iostream>
#include "physics/Box.hpp"
#include "physics/Ground.hpp"
#include "physics/ContactListener.hpp"
//#include "physics/Mouse.hpp"
//#include "physics/Keyboard.hpp"
enum _moveState {
    MS_STOP,
    MS_LEFT,
    MS_RIGHT,
    MS_UP
  };
_moveState moveState, moveState2;
bool mousePressed = false;
//static int numFootContacts;
ContactListener contactListener;
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
static void cursorPositionCallback(GLFWwindow* window, double xPos, double yPos);
void cursorEnterCallback(GLFWwindow* window, int entered);
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);


int main()
{
    std::vector<Box> boxes;
    b2Vec2 gravity(0.0f, -0.02f);
    b2World *world = new b2World(gravity);
    int boxIndex = 5;
    glm::vec3 green = {0.0f, 1.0f, 0.0f};
    glm::vec3 red = {1.0f, 0.0f, 0.0f};
    Renderer app;
    
    
    glfwSetKeyCallback(app.GetWindowHandle(), keyCallback);
    glfwSetCursorPosCallback(app.GetWindowHandle(), cursorPositionCallback);
    glfwSetInputMode(app.GetWindowHandle(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSetCursorEnterCallback(app.GetWindowHandle(), cursorEnterCallback);
    glfwSetMouseButtonCallback(app.GetWindowHandle(), mouseButtonCallback);
    //Mouse mouse(app);
    //Keyboard keyboard(app);
    const std::vector<uint16_t> indices = {
        0, 1, 2, 2, 3, 0};
    
    world->SetGravity(gravity);

    Ground ground(1, 20.0f, 1.0f, 0.0f, -40.0f, world, green);
    Box box(2, 1.0f, 1.0f, -0.45f, 3.55f, world, red, false);
    Ground ground2(3, 10.0f, 1.0f, 20.0f, 10.0f, world, green);
    Box human(4, 1.0f, 2.0f, 20.0f, 25.0f, world, red, true);
    Ground ground3(5, 5.0f, 1.5f, -8.0f, 4.0f, world, green);
    Ground ground4(6, 15.0f, 1.0f, -15.0f, -30.0f, world, green);
    Ground ground5(7, 20.0f, 1.0f, -10.0f, -20.0f, world, green);
    Box player2(8, 1.0f, 2.0f, 20.0f, 20.0f, world, red, true);
    Ground wall_left(9, 1.0f, 40.0f, -54.0f, 0.0f, world, green);
    Ground wall_right(10, 1.0f, 40.0f, 54.0f, 0.0f, world, green);

    

    app.RenderModelCreate(ground.ID, Renderer::RenderModel(ground.matrix, indices));
    app.RenderTargetCreate(1, ground.ID);

    app.RenderModelCreate(box.ID, Renderer::RenderModel(box.matrix, indices));
    app.RenderTargetCreate(2, box.ID);

    app.RenderModelCreate(ground2.ID, Renderer::RenderModel(ground2.matrix, indices));
    app.RenderTargetCreate(3, ground2.ID);

    app.RenderModelCreate(human.ID, Renderer::RenderModel(human.matrix, indices));
    app.RenderTargetCreate(4, human.ID);

    app.RenderModelCreate(ground3.ID, Renderer::RenderModel(ground3.matrix, indices));
    app.RenderTargetCreate(5, ground3.ID);

    app.RenderModelCreate(ground4.ID, Renderer::RenderModel(ground4.matrix, indices));
    app.RenderTargetCreate(6, ground4.ID);

    app.RenderModelCreate(ground5.ID, Renderer::RenderModel(ground5.matrix, indices));
    app.RenderTargetCreate(7, ground5.ID);

    app.RenderModelCreate(player2.ID, Renderer::RenderModel(player2.matrix, indices));
    app.RenderTargetCreate(8, player2.ID);

    app.RenderModelCreate(wall_left.ID, Renderer::RenderModel(wall_left.matrix, indices));
    app.RenderTargetCreate(9, wall_left.ID);

    app.RenderModelCreate(wall_right.ID, Renderer::RenderModel(wall_right.matrix, indices));
    app.RenderTargetCreate(10, wall_right.ID);
    
    
    for(int i = 0; i <boxes.size(); i++){
        app.RenderModelCreate(boxes.at(i).ID, Renderer::RenderModel(boxes.at(i).matrix, indices));
        app.RenderTargetCreate(i+5, boxes.at(i).ID);
    }

    float timeStep = 1.0f / 60.0f;
    int32 velocityIterations = 6;
    int32 positionIterations = 2;
    app.ViewTransformUpdate(glm::lookAt(glm::vec3(0.0f, 0.0f, 100.0f), glm::vec3(0.0f, 0.0f, 0.0f),glm::vec3(0.0f, 1.0f, 0.0f)));
    

    while (!glfwWindowShouldClose(app.GetWindowHandle()))
    {
        world->Step(timeStep, velocityIterations, positionIterations);
        world->SetContactListener(&contactListener);
        //printf("wsp x: %4.2f, wsp y: %4.2f\n", box.getBody()->GetPosition().x, box.getBody()->GetPosition().y);
        //printf("wsp x: %4.2f, wsp y: %4.2f\n", ground.getBody()->GetPosition().x, ground.getBody()->GetPosition().y);
        //app.RenderTargetUpdate(ground.ID, glm::translate(glm::mat4(1.0f), glm::vec3(ground.getBody()->GetPosition().x, ground.getBody()->GetPosition().y, 0.0f)));
        
        // if(mousePressed){
        //     double xPos = 0, yPos = 0;
        //     glfwGetCursorPos(app.GetWindowHandle(), &xPos, &yPos);
        //     int width = 800, height = 600;
        //     //app.GetWindowHandle()->glfwGetWindowSize(&width, &height);
        //     float MouseRelativeToCenterX = xPos - width;
        //     float MouseRelativeToCenterY = yPos - height;
        //     Box a(boxIndex, 1.0f, 1.0f, MouseRelativeToCenterX, MouseRelativeToCenterY, world, red, false);
        //     boxes.push_back(a);
        //     boxIndex++;
        // }
        app.RenderTargetUpdate(box.ID, glm::translate(glm::mat4(1.0f), glm::vec3(box.getBody()->GetPosition().x, box.getBody()->GetPosition().y, 0.0f)));
        app.RenderTargetUpdate(ground.ID, glm::translate(glm::mat4(1.0f), glm::vec3(ground.getBody()->GetPosition().x, ground.getBody()->GetPosition().y, 0.0f)));
        app.RenderTargetUpdate(ground2.ID, glm::translate(glm::mat4(1.0f), glm::vec3(ground2.getBody()->GetPosition().x, ground2.getBody()->GetPosition().y, 0.0f)));
        app.RenderTargetUpdate(ground3.ID, glm::translate(glm::mat4(1.0f), glm::vec3(ground3.getBody()->GetPosition().x, ground3.getBody()->GetPosition().y, 0.0f)));
        app.RenderTargetUpdate(ground4.ID, glm::translate(glm::mat4(1.0f), glm::vec3(ground4.getBody()->GetPosition().x, ground4.getBody()->GetPosition().y, 0.0f)));
        app.RenderTargetUpdate(ground5.ID, glm::translate(glm::mat4(1.0f), glm::vec3(ground5.getBody()->GetPosition().x, ground5.getBody()->GetPosition().y, 0.0f)));
        app.RenderTargetUpdate(wall_left.ID, glm::translate(glm::mat4(1.0f), glm::vec3(wall_left.getBody()->GetPosition().x, wall_left.getBody()->GetPosition().y, 0.0f)));
        app.RenderTargetUpdate(wall_right.ID, glm::translate(glm::mat4(1.0f), glm::vec3(wall_right.getBody()->GetPosition().x, wall_right.getBody()->GetPosition().y, 0.0f)));


        b2Vec2 vel = human.getBody()->GetLinearVelocity();
        b2Vec2 vel2 = player2.getBody()->GetLinearVelocity();

        switch (moveState){
            case MS_LEFT:  vel.x = -2; break;
            case MS_STOP:  vel.x =  0; break;
            case MS_RIGHT: vel.x =  2; break;
            case MS_UP: vel.y = 0.75; break;
        }
        switch (moveState2){
            case MS_LEFT:  vel2.x = -2; break;
            case MS_STOP:  vel2.x =  0; break;
            case MS_RIGHT: vel2.x =  2; break;
            case MS_UP: vel2.y = 0.75; break;
        }
        human.getBody()->SetLinearVelocity(vel);
        player2.getBody()->SetLinearVelocity(vel2);

        app.RenderTargetUpdate(human.ID, glm::translate(glm::mat4(1.0f), glm::vec3(human.getBody()->GetPosition().x, human.getBody()->GetPosition().y, 0.0f)));
        app.RenderTargetUpdate(player2.ID, glm::translate(glm::mat4(1.0f), glm::vec3(player2.getBody()->GetPosition().x, player2.getBody()->GetPosition().y, 0.0f)));

        for(int i = 0; i < boxes.size(); i++){
            app.RenderTargetUpdate(boxes.at(i).ID, glm::translate(glm::mat4(1.0f), glm::vec3(boxes.at(i).getBody()->GetPosition().x, boxes.at(i).getBody()->GetPosition().y, 0.0f)));
        }

        // for(b2Contact* contact = world->GetContactList(); contact; contact = contact->GetNext()){
        //     if(contact){
        //         printf("wykryto kontakt\n");
        //         bool var = contact->IsTouching();
        //         printf("%d\n", var);
        //         printf("wsp x ground: %4.2f, wsp y ground: %4.2f\n", ground.getBody()->GetPosition().x, ground.getBody()->GetPosition().y);
        //     }
        // }

        
        glfwPollEvents();
        app.Draw();
    }

    return 0;
}
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods){
    switch(key)
    {
        case GLFW_KEY_A:
        if(action == GLFW_PRESS || action == GLFW_REPEAT){
            moveState = MS_LEFT;
        }
        if(action == GLFW_RELEASE){
            moveState = MS_STOP;
        }
        break;
        case GLFW_KEY_D:
        if(action == GLFW_PRESS || action == GLFW_REPEAT){
            moveState = MS_RIGHT;
        }
        if(action == GLFW_RELEASE){
            moveState = MS_STOP;
        }
        break;
        case GLFW_KEY_W:
        if(action == GLFW_PRESS){
            //if(contactListener.number < 1) break;
            moveState = MS_UP;
            break;
        }
        case GLFW_KEY_J:
        if(action == GLFW_PRESS || action == GLFW_REPEAT){
            moveState2 = MS_LEFT;
        }
        if(action == GLFW_RELEASE){
            moveState2 = MS_STOP;
        }
        break;
        case GLFW_KEY_L:
        if(action == GLFW_PRESS || action == GLFW_REPEAT){
            moveState2 = MS_RIGHT;
        }
        if(action == GLFW_RELEASE){
            moveState2 = MS_STOP;
        }
        break;
        case GLFW_KEY_I:
        if(action == GLFW_PRESS){
            //if(contactListener.number < 1) break;
            moveState2 = MS_UP;
            break;
        }
    }
}
static void cursorPositionCallback(GLFWwindow* window, double xPos, double yPos){

}
void cursorEnterCallback(GLFWwindow* window, int entered){
    if(entered){
        std::cout << "Entered Window" << std::endl;
    }
    else std::cout << "Left Window" << std::endl;
}
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods){
    if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        mousePressed = true;
    }
    if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
    {
        mousePressed = false;
    }
}

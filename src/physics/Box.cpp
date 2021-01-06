
#include "Box.hpp"

Box::Box(float w_, float h_, float x_, float y_, b2World* world){
    w = w_;
    h = h_;
    x = x_;
    y = y_;
    numContacts = 0;
    createBody(world);
    makeMatrix();
    body->SetUserData(this);
}

void Box::createBody(b2World* world){
    b2BodyDef myBodyDef;
    myBodyDef.type = b2_dynamicBody;
    myBodyDef.position.Set(x, y);
    body = world->CreateBody(&myBodyDef);

    b2PolygonShape shape;
    shape.SetAsBox(w/2.0f, h/2.0f);
    b2FixtureDef myFixtureDef;
    myFixtureDef.shape = &shape;
    myFixtureDef.density = 1.0f;
    myFixtureDef.friction = 0.3f;
    body->CreateFixture(&myFixtureDef);
    
}

void Box::makeMatrix(){
    glm::vec3 color = {1.0f, 0.0f, 0.0f};
    matrix = {
        {{-k*w, -k*h}, color},
        {{k*w, -k*h}, color},
        {{k*w, k*h}, color},
        {{-k*w, k*h}, color}
    };
}
b2Body* Box::getBody(){
    return body;
}
int Box::getNumContacts(){return numContacts;}
void Box::startContact() {numContacts++;}
void Box::endContact() {numContacts--;}

#include "Box.hpp"

Box::Box(int ID_, float w_, float h_, float x_, float y_, b2World* world, glm::vec3 color, bool isPlayer_){
    w = w_;
    h = h_;
    x = x_;
    y = y_;
    ID = ID_;
    isPlayer = isPlayer_;
    numContacts = 0;
    createBody(world);
    makeMatrix(color);
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

    // if(isPlayer){
    //     shape.SetAsBox(0.3, 0.3, b2Vec2(0, -h/2.0f), 0);
    //     myFixtureDef.isSensor = true;
    //     b2Fixture* footSensorFixture = body->CreateFixture(&myFixtureDef);
    //     footSensorFixture->SetUserData((void*) 3);
    // }
    
}

void Box::makeMatrix(glm::vec3 color){
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
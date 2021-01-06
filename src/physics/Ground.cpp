#include "Ground.hpp"

Ground::Ground(float w_, float h_, float x_, float y_, b2World* world){
    w = w_;
    h = h_;
    x = x_;
    y = y_;
    createBody(world);
    makeMatrix();
}

void Ground::createBody(b2World* world){
    b2BodyDef myBodyDef;
    myBodyDef.type = b2_staticBody;
    myBodyDef.position.Set(x, y);
    body = world->CreateBody(&myBodyDef);

    b2PolygonShape shape;
    shape.SetAsBox(w, h);
    b2FixtureDef myFixtureDef;
    myFixtureDef.shape = &shape;
    body->CreateFixture(&myFixtureDef);

}

void Ground::makeMatrix(){
    glm::vec3 color = {0.0f, 1.0f, 0.0f};
    matrix = {
        {{-k*w, -k*h}, color},
        {{k*w, -k*h}, color},
        {{k*w, k*h}, color},
        {{-k*w, k*h}, color}
    };
}
b2Body* Ground::getBody(){
    return body;
}
void Ground::startContact() {numContacts++;}
void Ground::endContact() {numContacts--;}
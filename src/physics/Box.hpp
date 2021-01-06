#pragma once
#include <vector>

#include "../render/Renderer.hpp"
#include "box2d/box2d.h"


class Box{

    b2Body* body;
    float w, h;
    float x, y;
    float k = 1.0;
    int numContacts;
    bool isPlayer;

    public:
    std::vector<Renderer::Vertex> matrix;

    Box(float w_, float h_, float x_, float y_, b2World* world);
    void createBody(b2World* world);
    void makeMatrix();
    b2Body* getBody();
    int getNumContacts();
    void startContact();
    void endContact();

};
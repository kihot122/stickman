#pragma once
#include <vector>

#include "../render/Renderer.hpp"
#include <box2d/box2d.h>

class Ground{

    b2Body* body;
    float w, h;
    float x, y;
    float k = 1.0;
    int numContacts;


    public:
    int ID;
    std::vector<Renderer::Vertex> matrix;
    Ground(int ID_, float w_, float h_, float x_, float y_, b2World* world, glm::vec3 color);
    void createBody(b2World* world);
    void makeMatrix(glm::vec3 color);
    b2Body* getBody();
    void startContact();
    void endContact();
};
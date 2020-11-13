#pragma once
#include <iostream>
#include <vector>
#include <box2d/box2d.h>

class Base{
private:
    std::vector<b2BodyDef> handler_container;
public:
    virtual void tick();
    virtual int getFeedback();
    virtual std::vector<uint8_t>* serialize() = 0;
    virtual b2BodyDef deserialize() = 0;
};
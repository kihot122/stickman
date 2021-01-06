#pragma once

#include <vector>

#include "glm/glm.hpp"
#include "box2d/box2d.h"
#include "../render/Renderer.hpp"
#include "../physics/Ground.hpp"
#include "../physics/Box.hpp"

enum class EntityType
{
    WALL,
    PLAYER,
    BULLET,
};

class Entity
{
protected:
    EntityType Type;
    uint16_t ModelID;
    uint16_t TargetID;
    bool Dirty;
    bool Done;

public:
    virtual ~Entity(){};

    virtual void Tick() = 0;
    virtual std::vector<Renderer::Vertex> ModelCreateVertices() = 0;
    virtual std::vector<uint16_t> ModelCreateIndices() = 0;
    virtual glm::mat4 TargetUpdate() = 0;

    EntityType GetType();
    uint16_t GetModelID();
    uint16_t GetTargetID();
    bool IsDirty();
    bool IsDone();
};

class EntityWall : public Entity
{
    Ground* wall;

public:
    EntityWall(uint16_t modelID, uint16_t targetID, float weight, float height, float xPos, float yPos, b2World* world, bool dirty);

    virtual void Tick();
    virtual std::vector<Renderer::Vertex> ModelCreateVertices();
    virtual std::vector<uint16_t> ModelCreateIndices();
    virtual glm::mat4 TargetUpdate();
    Ground* getGroundEntity();
};

class EntityPlayer : public Entity
{
    Box* player;
public:
    EntityPlayer(uint16_t modelID, uint16_t targetID, float weight, float height, float xPos, float yPos, b2World* world, bool dirty);
    virtual void Tick();
    virtual std::vector<Renderer::Vertex> ModelCreateVertices();
    virtual std::vector<uint16_t> ModelCreateIndices();
    virtual glm::mat4 TargetUpdate();
    Box* getGroundEntity();
};

// class EntityBullet : public EntityWall
// {
// public:
//     EntityBullet();
// };
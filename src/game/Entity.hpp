#pragma once

#include <vector>

#include "glm/glm.hpp"

#include "../render/Renderer.hpp"

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

public:
    virtual void Tick() = 0;
    virtual std::vector<Renderer::Vertex> ModelCreateVertices() = 0;
    virtual std::vector<uint16_t> ModelCreateIndices() = 0;
    virtual glm::mat4 TargetUpdate() = 0;

    EntityType GetType();
    uint16_t GetModelID();
    uint16_t GetTargetID();
};

class EntityWall : public Entity
{
public:
    EntityWall();

    virtual void Tick();
    virtual std::vector<Renderer::Vertex> ModelCreateVertices();
    virtual std::vector<uint16_t> ModelCreateIndices();
    virtual glm::mat4 TargetUpdate();
};

class EntityPlayer : public EntityWall
{
public:
    EntityPlayer();
};

class EntityBullet : public EntityWall
{
public:
    EntityBullet();
};
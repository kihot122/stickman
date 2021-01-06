#include "Entity.hpp"

EntityType Entity::GetType()
{
    return Type;
}

uint16_t Entity::GetModelID()
{
    return ModelID;
}

uint16_t Entity::GetTargetID()
{
    return TargetID;
}

EntityWall::EntityWall(uint16_t modelID, uint16_t targetID)
bool Entity::IsDirty()
{
    return Dirty;
}

bool Entity::IsDone()
{
    return Done;
}

EntityWall::EntityWall()
{
    Type = EntityType::WALL;
    ModelID = modelID;
    TargetID = targetID;
}

void EntityWall::Tick()
{
}

std::vector<Renderer::Vertex> EntityWall::ModelCreateVertices()
{
    return std::vector<Renderer::Vertex>{
        {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
        {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
        {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}};
}

std::vector<uint16_t> EntityWall::ModelCreateIndices()
{
    return std::vector<uint16_t>{0, 1, 2, 2, 3, 0};
}

glm::mat4 EntityWall::TargetUpdate()
{
    return glm::rotate(glm::mat4(1.0f), glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
}

EntityPlayer::EntityPlayer()
{
    Type = EntityType::PLAYER;
}

EntityBullet::EntityBullet()
{
    Type = EntityType::BULLET;
}
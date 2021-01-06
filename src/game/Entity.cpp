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

bool Entity::IsDirty()
{
    return Dirty;
}

bool Entity::IsDone()
{
    return Done;
}

EntityWall::EntityWall(uint16_t modelID, uint16_t targetID, float weight, float height, float xPos, float yPos, b2World* world, bool dirty)
{
    Type = EntityType::WALL;
    ModelID = modelID;
    TargetID = targetID;
    Dirty = dirty;
    Done = false;
    wall = Ground(modelID, weight, height, xPos, yPos, world);
}

void EntityWall::Tick()
{
}

std::vector<Renderer::Vertex> EntityWall::ModelCreateVertices()
{
    return wall.matrix;
}

std::vector<uint16_t> EntityWall::ModelCreateIndices()
{
    return std::vector<uint16_t>{0, 1, 2, 2, 3, 0};
}

glm::mat4 EntityWall::TargetUpdate()
{
    return glm::translate(glm::mat4(1.0f), glm::vec3(wall.getBody()->GetPosition().x, wall.getBody()->GetPosition().y, 0.0f));
}

Ground getGroundEntity() {return wall;}

EntityPlayer::EntityPlayer(uint16_t modelID, uint16_t targetID, float weight, float height, float xPos, float yPos, b2World* world, bool dirty)
{
    Type = EntityType::PLAYER;
    ModelID = modelID;
    TargetID = targetID;
    Dirty = dirty;
    Done = false;
    player = Box(modelID, weight, height, xPos, yPos, world);
}
void EntityPlayer::Tick()
{
}

std::vector<Renderer::Vertex> EntityPlayer::ModelCreateVertices()
{
    return player.matrix;
}

std::vector<uint16_t> EntityPlayer::ModelCreateIndices()
{
    return std::vector<uint16_t>{0, 1, 2, 2, 3, 0};
}

glm::mat4 EntityPlayer::TargetUpdate()
{
    return glm::translate(glm::mat4(1.0f), glm::vec3(player.getBody()->GetPosition().x, player.getBody()->GetPosition().y, 0.0f));
}

Box getGroundEntity() {return player;}

EntityBullet::EntityBullet()
{
    Type = EntityType::BULLET;
}
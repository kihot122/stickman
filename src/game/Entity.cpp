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

void Entity::Kill()
{
    Done = true;
}

EntityWall::EntityWall(uint16_t modelID, uint16_t targetID, float weight, float height, float xPos, float yPos, b2World *world, bool dirty)
{
    Type = EntityType::WALL;
    ModelID = modelID;
    TargetID = targetID;
    Dirty = dirty;
    Done = false;
    wall = new Ground(weight, height, xPos, yPos, world);
}

void EntityWall::Tick()
{
}

std::vector<Renderer::Vertex> EntityWall::ModelCreateVertices()
{
    return wall->matrix;
}

std::vector<uint16_t> EntityWall::ModelCreateIndices()
{
    return std::vector<uint16_t>{0, 1, 2, 2, 3, 0};
}

glm::mat4 EntityWall::TargetUpdate()
{
    return glm::translate(glm::mat4(1.0f), glm::vec3(wall->getBody()->GetPosition().x, wall->getBody()->GetPosition().y, 0.0f));
}

Ground *EntityWall::getGroundEntity() { return wall; }

EntityPlayer::EntityPlayer(uint16_t modelID, uint16_t targetID, float weight, float height, float xPos, float yPos, b2World *world, bool dirty, int socket_)
{
    Type = EntityType::PLAYER;
    ModelID = modelID;
    TargetID = targetID;
    Dirty = dirty;
    Done = false;
    socket = socket_;
    player = new Box(weight, height, xPos, yPos, world);
}
void EntityPlayer::Tick()
{

    if (player->getBody()->GetPosition().y < -54.0f)
        Done = true;
}

std::vector<Renderer::Vertex> EntityPlayer::ModelCreateVertices()
{
    return player->matrix;
}

std::vector<uint16_t> EntityPlayer::ModelCreateIndices()
{
    return std::vector<uint16_t>{0, 1, 2, 2, 3, 0};
}

glm::mat4 EntityPlayer::TargetUpdate()
{
    return glm::translate(glm::mat4(1.0f), glm::vec3(player->getBody()->GetPosition().x, player->getBody()->GetPosition().y, 0.0f));
}
void EntityPlayer::SetSocketToPlayer()
{
}
int EntityPlayer::GetSocket() { return socket; }
Box *EntityPlayer::GetGroundEntity() { return player; }
void EntityPlayer::SetAction(int key)
{
    b2Vec2 vel = player->getBody()->GetLinearVelocity();
    switch (key)
    {
    case GLFW_KEY_A:
        vel.x = -50;
        break;
    case GLFW_KEY_D:
        vel.x = 50;
        break;
    case GLFW_KEY_W:
        vel.y = 50;
        break;
    case 1000:
        vel.x = 0;
        break;
    }
    player->getBody()->SetLinearVelocity(vel);
}

// EntityBullet::EntityBullet()
// {
//     Type = EntityType::BULLET;
// }
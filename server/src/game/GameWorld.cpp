#include "GameWorld.hpp"

#include <box2d/b2_chain_shape.h>
#include <box2d/b2_circle_shape.h>
#include <box2d/b2_fixture.h>
#include <box2d/b2_polygon_shape.h>
#include <box2d/b2_shape.h>

#include "extension/b2MathExtension.hpp"

using namespace game::extension;

namespace game
{

GameWorld::GameWorld() : m_playerList{}, m_world({0.0f, 0.0f})
{
    // TODO: Fix this
    m_playerList[0].state = PlayerEntity::State::Dead;

    m_map = map::MapData::getDefault();
}

void GameWorld::addPlayer(uint id)
{
    // TODO
}

void GameWorld::initialize()
{
    const float mapWidth = m_map.width();
    const float mapHeight = m_map.height();

    // Create ground
    b2BodyDef groundBodyDef;
    groundBodyDef.position.Set(0.0f, 0.0f);

    b2Vec2 groundVertices[4] = {{0.0f, 0.0f}, {0.0f, mapHeight}, {mapWidth, mapHeight}, {mapWidth, 0.0f}};
    b2ChainShape groundShape;
    groundShape.CreateLoop(groundVertices, 4);

    b2Body* groundBody = m_world.CreateBody(&groundBodyDef);
    groundBody->CreateFixture(&groundShape, 0.0f);

    // Reusable tile Fixture
    b2PolygonShape tileShape;
    tileShape.SetAsBox(0.5f, 0.5f);

    b2FixtureDef tileFixtureDef;
    tileFixtureDef.shape = &tileShape;
    tileFixtureDef.density = 0.0f;
    tileFixtureDef.friction = 0.0f;

    // Populate map
    const int width = m_map.width();
    const int height = m_map.height();
    const map::TileType* tiles = m_map.tiles();
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            map::TileType tile = tiles[x + y * height];

            switch (tile)
            {
            case map::TileType::Empty:
                break;
            case map::TileType::Wall:
            case map::TileType::Breakable:
                tileShape.SetAsBox(0.5f, 0.5f, b2Vec2{0.5f + x, 0.5f + y}, 0.0f);
                groundBody->CreateFixture(&tileFixtureDef);
                break;
            default:
                break;
            }
        }
    }

    // Create players
    b2BodyDef playerBodyDef;
    playerBodyDef.type = b2_dynamicBody;
    playerBodyDef.fixedRotation = true;

    b2CircleShape playerColliderShape;
    playerColliderShape.m_p.Set(0.0f, 0.0f);
    playerColliderShape.m_radius = 0.4f;

    b2FixtureDef playerColliderFixtureDef;
    playerColliderFixtureDef.shape = &playerColliderShape;
    playerColliderFixtureDef.density = 1.0f;
    playerColliderFixtureDef.friction = 0.3f;
    for (size_t i = 0; i < m_playerList.size(); i++)
    {
        if (m_playerList[i].state == PlayerEntity::State::Invalid)
        {
            continue;
        }
        PlayerEntity& playerEntity = m_playerList[i];

        playerBodyDef.position.Set(5.0f + 2 * i, 10.0f);
        playerBodyDef.userData.pointer = reinterpret_cast<uintptr_t>(&playerEntity);

        b2Body* body = m_world.CreateBody(&playerBodyDef);
        body->CreateFixture(&playerColliderFixtureDef);

        playerEntity.body = body;

        playerEntity.state = PlayerEntity::State::Alive;
    }
}

PlayerEntity::InputData& GameWorld::playerInput(uint id)
{
    return m_playerList[id].inputData;
}

const PlayerEntity& GameWorld::playerEntity(uint id) const
{
    return m_playerList[id];
}

void GameWorld::applyPlayerInput()
{
    constexpr float speed = 6.0f;

    for (auto& player : m_playerList)
    {
        if (player.state != PlayerEntity::State::Alive)
        {
            continue;
        }

        player.body->SetLinearVelocity(normalized(player.inputData.direction) * speed);
    }
}

void GameWorld::step(time_duration deltaTime)
{
    m_world.Step(deltaTime.count(), 6, 2);
}

} // namespace game

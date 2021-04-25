#pragma once

#include "core.hpp"
#include "PlayerEntity.hpp"
#include "map/MapData.hpp"

#include <box2d/b2_world.h>

#include <array>

namespace game
{

class GameWorld
{
public:
    GameWorld();

    void addPlayer(uint id);
    void initialize();

    PlayerEntity::InputData& playerInput(uint id);
    const PlayerEntity& playerEntity(uint id) const;

    void applyPlayerInput();
    void step(time_duration deltaTime);

private:
    std::array<PlayerEntity, g_maxPlayerCount> m_playerList;

    map::MapData m_map;

    b2World m_world;
};

} // namespace game

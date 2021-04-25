#pragma once

#include <box2d/b2_math.h>
#include <box2d/b2_body.h>

namespace game
{

struct PlayerEntity
{
    enum class State
    {
        Invalid = 0,
        Dead = 1,
        Alive = 2
    };
    struct InputData
    {
        b2Vec2 direction;
        bool placeBomb;
    };
    struct StateData
    {
        uint8_t maxBombCount;
        uint8_t carryingBombCount;
    };

    
    State state;

    b2Body* body;

    InputData inputData;
    StateData stateData;
};

} // namespace game

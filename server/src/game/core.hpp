#pragma once

#include <chrono>
#include <sys/types.h>

namespace game
{

using hrclock = std::chrono::high_resolution_clock;
using hrc_time_point = hrclock::time_point;
using time_duration = std::chrono::duration<float>;
constexpr uint g_maxPlayerCount = 4;

} // namespace game

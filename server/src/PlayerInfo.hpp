#pragma once

#include "IPEndpoint.hpp"

#include <string>

class PlayerInfo
{
public:
    PlayerInfo() = default;

    PlayerInfo(std::string name, IPEndpoint endpoint) : m_name(std::move(name)), m_endpoint(std::move(endpoint))
    {
    }

    const std::string& name() const
    {
        return m_name;
    }

private:
    std::string m_name;
    IPEndpoint m_endpoint;
};
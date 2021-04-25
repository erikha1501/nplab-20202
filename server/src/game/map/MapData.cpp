#include "MapData.hpp"

#include <cassert>
#include <cstring>
#include <fstream>

namespace game::map
{

bool MapData::s_cacheValid = false;
MapData MapData::s_cachedMap = MapData{};

MapData MapData::getDefault()
{
    if (!s_cacheValid)
    {
        s_cachedMap = loadFromFile(s_defaultMapFileName);
    }

    return s_cachedMap.clone();
}

MapData MapData::loadFromFile(const char* fileName)
{
    std::ifstream mapFile{fileName, std::ios_base::binary};

    int width, height;
    mapFile.read((char*)&width, sizeof(int));
    mapFile.read((char*)&height, sizeof(int));
    assert((width > 0) && (height > 0));

    MapData result{width, height};

    mapFile.read((char*)result.data(), width * height);

    return result;
}

void MapData::saveToFile(const MapData& mapData, const char* fileName)
{
    std::ofstream mapFile{fileName, std::ios_base::binary};
    int width = mapData.m_width;
    int height = mapData.m_height;

    assert((width > 0) && (height > 0));
    mapFile.write((char*)&width, sizeof(int));
    mapFile.write((char*)&height, sizeof(int));

    mapFile.write((char*)mapData.data(), mapData.m_width * mapData.m_height);
}


TileType& MapData::at(int x, int y)
{
    return reinterpret_cast<TileType&>(m_tiles[x + m_width * y]);
}
TileType MapData::at(int x, int y) const
{
    return (TileType)m_tiles[x + m_width * y];
}

void MapData::clear()
{
    std::memset(m_tiles.get(), 0, m_width * m_height);
}

MapData MapData::clone() const
{
    MapData result{this->m_width, this->m_height};
    std::memcpy(result.data(), this->data(), this->m_width * this->m_height);

    return result;
}

} // namespace game::map

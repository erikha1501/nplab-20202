#pragma once

#include <memory>

namespace game::map
{

enum class TileType : uint8_t
{
    Empty = 0,
    Wall = 1,
    Breakable = 2
};

class MapData
{
public:
    MapData() : m_width(0), m_height(0), m_tiles{}
    {
    }

    MapData(int width, int height) : m_width(width), m_height(height), m_tiles(new uint8_t[width * height])
    {
    }

    MapData(const MapData&) = delete;
    MapData& operator=(const MapData&) = delete;

    MapData(MapData&&) = default;
    MapData& operator=(MapData&&) = default;

public:
    static inline const char* s_defaultMapFileName = "default.bomap";
    static MapData getDefault();

    static MapData loadFromFile(const char* fileName);
    static void saveToFile(const MapData& mapData, const char* fileName);

    int width() const
    {
        return m_width;
    }
    int height() const
    {
        return m_height;
    }

    const uint8_t* data() const
    {
        return m_tiles.get();
    }
    uint8_t* data()
    {
        return m_tiles.get();
    }

    const TileType* tiles() const
    {
        return (TileType*)(m_tiles.get());
    }
    TileType* tiles()
    {
        return (TileType*)(m_tiles.get());
    }

    TileType& at(int x, int y);
    TileType at(int x, int y) const;

    void clear();

    MapData clone() const;

private:
    static MapData s_cachedMap;
    static bool s_cacheValid;

    int m_width;
    int m_height;

    std::unique_ptr<uint8_t[]> m_tiles;
};

} // namespace game::map

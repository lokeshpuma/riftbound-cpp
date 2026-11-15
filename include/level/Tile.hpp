#pragma once

#include "core/Types.hpp"
#include "physics/AABB.hpp"
#include <raylib.h>

namespace riftbound {

constexpr int TILE_SIZE = 24;

struct Tile {
    TileType type = TileType::Air;
    int gridX = 0;
    int gridY = 0;

    AABB GetBounds() const {
        return AABB{
            static_cast<float>(gridX * TILE_SIZE),
            static_cast<float>(gridY * TILE_SIZE),
            static_cast<float>(TILE_SIZE),
            static_cast<float>(TILE_SIZE)
        };
    }
};

} // namespace riftbound

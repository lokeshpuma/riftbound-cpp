#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>
#include <raylib.h>

namespace riftbound {

enum class Direction : int8_t {
    Left = -1,
    None = 0,
    Right = 1
};

enum class WeaponType : uint8_t {
    EmberBlade,
    PulseBolts
};

enum class EntityType : uint8_t {
    Player,
    Patroller,
    RangedSentry,
    Chaser,
    Boss,
    Projectile,
    Collectible,
    Hazard,
    Checkpoint
};

enum class CollectibleType : uint8_t {
    Health,
    Energy,
    EmberCore
};

enum class TileType : uint8_t {
    Air = 0,
    SolidGround = 1,
    SolidStone = 2,
    SolidBrick = 3,
    OneWayPlatform = 4,
    HazardSpikes = 5,
    HazardLava = 6,
    DecorativePillar = 7
};

enum class GameStateId : uint8_t {
    Boot,
    MainMenu,
    LevelSelect,
    Playing,
    Paused,
    Settings,
    GameOver,
    LevelComplete,
    Victory
};

} // namespace riftbound

#pragma once

#include "physics/AABB.hpp"
#include <cstdint>

namespace riftbound {

enum class CollisionLayer : uint16_t {
    None        = 0,
    Solid       = 1 << 0,
    OneWay      = 1 << 1,
    Hazard      = 1 << 2,
    Player      = 1 << 3,
    Enemy       = 1 << 4,
    Projectile  = 1 << 5,
    Trigger     = 1 << 6
};

inline CollisionLayer operator|(CollisionLayer a, CollisionLayer b) {
    return static_cast<CollisionLayer>(static_cast<uint16_t>(a) | static_cast<uint16_t>(b));
}

inline bool operator&(CollisionLayer a, CollisionLayer b) {
    return (static_cast<uint16_t>(a) & static_cast<uint16_t>(b)) != 0;
}

struct CollisionResult {
    bool collidedX = false;
    bool collidedY = false;
    bool grounded = false;
    bool hitCeiling = false;
};

struct PlatformCollider {
    AABB bounds;
    bool isOneWay = false;
    bool isHazard = false;
    float hazardDamage = 20.0f;
};

} // namespace riftbound

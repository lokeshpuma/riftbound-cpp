#pragma once

#include "core/Types.hpp"
#include "physics/AABB.hpp"
#include <raylib.h>

namespace riftbound {

class Collectible {
public:
    Collectible(Vector2 pos, CollectibleType type);

    void Update(float dt);
    void Draw() const;
    bool CheckPickup(const AABB& playerBox);

    bool IsActive() const { return m_active; }
    CollectibleType GetType() const { return m_type; }
    Vector2 GetPosition() const { return m_position; }
    AABB GetBounds() const;

private:
    Vector2 m_position;
    Vector2 m_basePos;
    CollectibleType m_type;
    bool m_active = true;
    float m_animTimer = 0.0f;
    float m_radius = 8.0f;
};

} // namespace riftbound

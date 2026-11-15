#pragma once

#include "physics/AABB.hpp"

namespace riftbound {

class Hazard {
public:
    Hazard(AABB box, float damage = 25.0f);

    void Update(float dt);
    bool CheckDamagePlayer(const AABB& playerBox, float& outDamage);

    AABB GetBounds() const { return m_bounds; }

private:
    AABB m_bounds;
    float m_damage = 25.0f;
    float m_cooldownTimer = 0.0f;
    float m_cooldownDuration = 1.0f; // Prevent damage every frame!
};

} // namespace riftbound

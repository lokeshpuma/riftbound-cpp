#include "level/Hazard.hpp"

namespace riftbound {

Hazard::Hazard(AABB box, float damage)
    : m_bounds(box),
      m_damage(damage) {
}

void Hazard::Update(float dt) {
    if (m_cooldownTimer > 0.0f) {
        m_cooldownTimer -= dt;
    }
}

bool Hazard::CheckDamagePlayer(const AABB& playerBox, float& outDamage) {
    if (m_cooldownTimer > 0.0f) return false;

    if (Intersects(playerBox, m_bounds)) {
        m_cooldownTimer = m_cooldownDuration;
        outDamage = m_damage;
        return true;
    }
    return false;
}

} // namespace riftbound

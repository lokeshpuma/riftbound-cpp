#include "entities/Entity.hpp"
#include <algorithm>

namespace riftbound {

uint32_t Entity::s_nextId = 1;

Entity::Entity(EntityType type, Vector2 pos, Vector2 size, float maxHp)
    : m_id(s_nextId++),
      m_type(type),
      m_position(pos),
      m_size(size),
      m_health(maxHp),
      m_maxHealth(maxHp) {
}

AABB Entity::GetBounds() const {
    return AABB{
        m_position.x - m_size.x * 0.5f,
        m_position.y - m_size.y * 0.5f,
        m_size.x,
        m_size.y
    };
}

bool Entity::TakeDamage(const DamageInfo& info) {
    if (!m_isAlive || m_invulnTimer > 0.0f) return false;

    m_health -= info.amount;
    m_invulnTimer = m_invulnDuration;

    // Apply knockback
    m_velocity.x += info.knockback.x;
    m_velocity.y += info.knockback.y;

    if (m_health <= 0.0f) {
        m_health = 0.0f;
        m_isAlive = false;
    }

    return true;
}

} // namespace riftbound

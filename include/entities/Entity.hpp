#pragma once

#include "core/Types.hpp"
#include "physics/AABB.hpp"
#include "combat/DamageInfo.hpp"
#include <raylib.h>
#include <cstdint>

namespace riftbound {

class PhysicsWorld;

class Entity {
public:
    Entity(EntityType type, Vector2 pos, Vector2 size, float maxHp);
    virtual ~Entity() = default;

    virtual void Update(float dt, PhysicsWorld& world) = 0;
    virtual void Draw() const = 0;
    virtual bool TakeDamage(const DamageInfo& info);

    uint32_t GetId() const { return m_id; }
    EntityType GetType() const { return m_type; }
    Vector2 GetPosition() const { return m_position; }
    Vector2 GetVelocity() const { return m_velocity; }
    Vector2 GetSize() const { return m_size; }
    AABB GetBounds() const;
    Direction GetFacingDirection() const { return m_facing; }

    float GetHealth() const { return m_health; }
    float GetMaxHealth() const { return m_maxHealth; }
    bool IsAlive() const { return m_isAlive; }
    bool IsActive() const { return m_active; }

    void SetPosition(Vector2 pos) { m_position = pos; }
    void SetVelocity(Vector2 vel) { m_velocity = vel; }
    void SetFacingDirection(Direction dir) { m_facing = dir; }
    void SetActive(bool act) { m_active = act; }

protected:
    static uint32_t s_nextId;
    uint32_t m_id;
    EntityType m_type;

    Vector2 m_position;
    Vector2 m_velocity = { 0.0f, 0.0f };
    Vector2 m_size;
    Direction m_facing = Direction::Right;

    float m_health;
    float m_maxHealth;
    bool m_isAlive = true;
    bool m_active = true;

    float m_invulnTimer = 0.0f;
    float m_invulnDuration = 0.2f;
};

} // namespace riftbound

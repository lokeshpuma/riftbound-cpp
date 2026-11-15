#pragma once

#include "physics/AABB.hpp"
#include <raylib.h>

namespace riftbound {

class Projectile {
public:
    Projectile() = default;

    void Spawn(Vector2 pos, Vector2 vel, float damage, float lifetime, float radius, bool fromPlayer);
    void Update(float dt);
    void Draw() const;
    void Deactivate();

    bool IsActive() const { return m_active; }
    bool IsFromPlayer() const { return m_fromPlayer; }
    float GetDamage() const { return m_damage; }
    Vector2 GetPosition() const { return m_position; }
    Vector2 GetVelocity() const { return m_velocity; }
    AABB GetBounds() const;

private:
    Vector2 m_position = { 0.0f, 0.0f };
    Vector2 m_velocity = { 0.0f, 0.0f };
    float m_damage = 20.0f;
    float m_lifetime = 0.0f;
    float m_maxLifetime = 2.0f;
    float m_radius = 5.0f;
    bool m_fromPlayer = false;
    bool m_active = false;
};

} // namespace riftbound

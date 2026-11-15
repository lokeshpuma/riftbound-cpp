#pragma once

#include "combat/Weapon.hpp"
#include "physics/AABB.hpp"
#include <unordered_set>
#include <cstdint>

namespace riftbound {

class EmberBlade : public Weapon {
public:
    EmberBlade();

    void Update(float dt) override;
    bool Attack(Vector2 userPos, Direction facingDir) override;
    void Draw(Vector2 userPos, Direction facingDir) const override;
    WeaponType GetType() const override { return WeaponType::EmberBlade; }
    const char* GetName() const override { return "Ember Blade"; }

    bool IsSlashing() const { return m_slashActive; }
    AABB GetHitbox(Vector2 userPos, Direction facingDir) const;
    float GetDamage() const { return m_damage; }
    float GetKnockback() const { return m_knockback; }
    uint32_t GetCurrentAttackId() const { return m_attackId; }

    bool HasHitEntity(uint32_t entityId) const;
    void RegisterHitEntity(uint32_t entityId);

private:
    float m_damage = 40.0f;
    float m_reachX = 38.0f;
    float m_reachY = 32.0f;
    float m_knockback = 220.0f;

    bool m_slashActive = false;
    float m_slashTimer = 0.0f;
    float m_slashDuration = 0.16f;
    Direction m_slashDir = Direction::Right;
    Vector2 m_slashPos = { 0.0f, 0.0f };

    uint32_t m_attackId = 1;
    std::unordered_set<uint32_t> m_hitEntities;
};

} // namespace riftbound

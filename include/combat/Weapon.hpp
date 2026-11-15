#pragma once

#include "core/Types.hpp"
#include <raylib.h>

namespace riftbound {

class Weapon {
public:
    virtual ~Weapon() = default;

    virtual void Update(float dt) = 0;
    virtual bool Attack(Vector2 userPos, Direction facingDir) = 0;
    virtual void Draw(Vector2 userPos, Direction facingDir) const = 0;
    virtual WeaponType GetType() const = 0;
    virtual const char* GetName() const = 0;

    bool CanAttack() const { return m_cooldownTimer <= 0.0f; }
    float GetCooldownProgress() const {
        if (m_cooldownDuration <= 0.0f) return 1.0f;
        return 1.0f - (m_cooldownTimer / m_cooldownDuration);
    }

protected:
    float m_cooldownTimer = 0.0f;
    float m_cooldownDuration = 0.25f;
};

} // namespace riftbound

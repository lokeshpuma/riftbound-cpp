#pragma once

#include "entities/Enemy.hpp"

namespace riftbound {

class RangedSentry : public Enemy {
public:
    RangedSentry(Vector2 pos, float detectionRange = 280.0f);

    void Update(float dt, PhysicsWorld& world) override;
    void Draw() const override;

    void SetTargetPosition(Vector2 target) { m_targetPos = target; }

private:
    float m_fireInterval = 2.2f;
    float m_fireTimer = 0.0f;
    float m_telegraphDuration = 0.45f;
    bool m_isTelegraphing = false;
    float m_projectileSpeed = 220.0f;
    float m_detectionRange = 280.0f;
    Vector2 m_targetPos = { 0.0f, 0.0f };
    Vector2 m_anchorPos;
    float m_animTimer = 0.0f;

    void FireProjectile();
};

} // namespace riftbound

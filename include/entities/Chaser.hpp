#pragma once

#include "entities/Enemy.hpp"

namespace riftbound {

class Chaser : public Enemy {
public:
    Chaser(Vector2 pos, float aggroRange = 190.0f);

    void Update(float dt, PhysicsWorld& world) override;
    void Draw() const override;

    void SetTargetPosition(Vector2 target) { m_targetPos = target; }

private:
    float m_speed = 110.0f;
    float m_aggroRange = 190.0f;
    bool m_isChasing = false;
    Vector2 m_targetPos = { 0.0f, 0.0f };
    float m_jumpCooldown = 0.0f;
    float m_animTimer = 0.0f;
};

} // namespace riftbound

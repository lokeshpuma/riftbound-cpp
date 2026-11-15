#pragma once

#include "entities/Enemy.hpp"

namespace riftbound {

class Patroller : public Enemy {
public:
    Patroller(Vector2 pos, float patrolLeft, float patrolRight);

    void Update(float dt, PhysicsWorld& world) override;
    void Draw() const override;

private:
    float m_patrolLeft;
    float m_patrolRight;
    float m_speed = 55.0f;
    float m_animTimer = 0.0f;
};

} // namespace riftbound

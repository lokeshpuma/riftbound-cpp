#pragma once

#include "physics/AABB.hpp"
#include <raylib.h>

namespace riftbound {

class Checkpoint {
public:
    Checkpoint(Vector2 pos);

    void Update(float dt);
    void Draw() const;
    bool CheckActivate(const AABB& playerBox);

    bool IsActivated() const { return m_activated; }
    Vector2 GetPosition() const { return m_position; }
    AABB GetBounds() const;

private:
    Vector2 m_position;
    bool m_activated = false;
    float m_glowTimer = 0.0f;
    float m_width = 24.0f;
    float m_height = 40.0f;
};

} // namespace riftbound

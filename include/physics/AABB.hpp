#pragma once

#include <raylib.h>
#include <algorithm>

namespace riftbound {

struct AABB {
    float x = 0.0f;
    float y = 0.0f;
    float width = 0.0f;
    float height = 0.0f;

    constexpr float Left() const { return x; }
    constexpr float Right() const { return x + width; }
    constexpr float Top() const { return y; }
    constexpr float Bottom() const { return y + height; }
    constexpr Vector2 Center() const { return { x + width * 0.5f, y + height * 0.5f }; }

    Rectangle ToRectangle() const {
        return Rectangle{ x, y, width, height };
    }

    static AABB FromRectangle(Rectangle r) {
        return AABB{ r.x, r.y, r.width, r.height };
    }

    static AABB FromCenterSize(Vector2 center, Vector2 size) {
        return AABB{ center.x - size.x * 0.5f, center.y - size.y * 0.5f, size.x, size.y };
    }
};

// Check if two AABBs overlap
bool Intersects(const AABB& a, const AABB& b);

// Check if AABB contains a point
bool Contains(const AABB& box, Vector2 point);

// Compute intersection depth (penetration) on both axes
Vector2 GetOverlapDepth(const AABB& a, const AABB& b);

// Compute swept broadphase box encompassing movement
AABB GetSweptBox(const AABB& box, Vector2 velocity, float dt);

} // namespace riftbound

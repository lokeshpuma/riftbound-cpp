#include "physics/AABB.hpp"
#include <cmath>
#include <algorithm>

namespace riftbound {

bool Intersects(const AABB& a, const AABB& b) {
    return (a.x < b.x + b.width &&
            a.x + a.width > b.x &&
            a.y < b.y + b.height &&
            a.y + a.height > b.y);
}

bool Contains(const AABB& box, Vector2 point) {
    return (point.x >= box.x &&
            point.x <= box.x + box.width &&
            point.y >= box.y &&
            point.y <= box.y + box.height);
}

Vector2 GetOverlapDepth(const AABB& a, const AABB& b) {
    float dx1 = (b.x + b.width) - a.x;
    float dx2 = (a.x + a.width) - b.x;
    float overlapX = std::min(dx1, dx2);

    float dy1 = (b.y + b.height) - a.y;
    float dy2 = (a.y + a.height) - b.y;
    float overlapY = std::min(dy1, dy2);

    return Vector2{ overlapX, overlapY };
}

AABB GetSweptBox(const AABB& box, Vector2 velocity, float dt) {
    AABB swept;
    float nextX = box.x + velocity.x * dt;
    float nextY = box.y + velocity.y * dt;

    swept.x = std::min(box.x, nextX);
    swept.y = std::min(box.y, nextY);
    swept.width = box.width + std::abs(velocity.x * dt);
    swept.height = box.height + std::abs(velocity.y * dt);

    return swept;
}

} // namespace riftbound

#include "physics/PhysicsWorld.hpp"
#include <cmath>
#include <algorithm>

namespace riftbound {

void PhysicsWorld::Clear() {
    m_platforms.clear();
}

void PhysicsWorld::AddSolid(AABB box) {
    m_platforms.push_back({ box, false, false, 0.0f });
}

void PhysicsWorld::AddOneWayPlatform(AABB box) {
    m_platforms.push_back({ box, true, false, 0.0f });
}

void PhysicsWorld::AddHazard(AABB box, float damage) {
    m_platforms.push_back({ box, false, true, damage });
}

CollisionResult PhysicsWorld::MoveAndSlide(AABB& box, Vector2& velocity, float dt, bool dropThroughOneWay) {
    CollisionResult result;
    if (dt <= 0.0f) return result;

    // Sub-stepping to prevent tunneling at high speeds
    const float maxStep = 8.0f; // maximum movement in pixels per sub-step
    float totalDx = velocity.x * dt;
    float totalDy = velocity.y * dt;
    float maxDist = std::max(std::abs(totalDx), std::abs(totalDy));
    int steps = static_cast<int>(std::ceil(maxDist / maxStep));
    if (steps < 1) steps = 1;

    float stepDx = totalDx / static_cast<float>(steps);
    float stepDy = totalDy / static_cast<float>(steps);

    for (int i = 0; i < steps; ++i) {
        // Resolve X movement
        if (std::abs(stepDx) > 0.0001f) {
            ResolveAxisX(box, velocity, stepDx, result);
        }

        // Resolve Y movement
        ResolveAxisY(box, velocity, stepDy, dropThroughOneWay, result);
    }

    return result;
}

void PhysicsWorld::ResolveAxisX(AABB& box, Vector2& velocity, float stepX, CollisionResult& result) {
    box.x += stepX;

    for (const auto& plat : m_platforms) {
        if (plat.isOneWay || plat.isHazard) continue; // One-way and hazards do not block X

        if (Intersects(box, plat.bounds)) {
            result.collidedX = true;
            if (stepX > 0.0f) {
                // Moving right, hit left edge of platform
                box.x = plat.bounds.x - box.width;
                velocity.x = 0.0f;
            } else if (stepX < 0.0f) {
                // Moving left, hit right edge of platform
                box.x = plat.bounds.x + plat.bounds.width;
                velocity.x = 0.0f;
            }
        }
    }
}

void PhysicsWorld::ResolveAxisY(AABB& box, Vector2& velocity, float stepY, bool dropThroughOneWay, CollisionResult& result) {
    float prevBottom = box.Bottom();
    box.y += stepY;

    for (const auto& plat : m_platforms) {
        if (plat.isHazard) continue;

        if (plat.isOneWay) {
            if (dropThroughOneWay) continue;
            // Only collide with one-way platform when falling downwards and previously above it
            if (stepY >= 0.0f && prevBottom <= plat.bounds.y + 4.0f && box.Bottom() >= plat.bounds.y) {
                // Check horizontal overlap
                if (box.Right() > plat.bounds.x + 1.0f && box.Left() < plat.bounds.Right() - 1.0f) {
                    box.y = plat.bounds.y - box.height;
                    velocity.y = 0.0f;
                    result.grounded = true;
                    result.collidedY = true;
                }
            }
        } else {
            // Full solid platform
            if (Intersects(box, plat.bounds)) {
                if (stepY > 0.0f) {
                    // Falling down, hit floor
                    box.y = plat.bounds.y - box.height;
                    velocity.y = 0.0f;
                    result.grounded = true;
                    result.collidedY = true;
                } else if (stepY < 0.0f) {
                    // Jumping up, hit ceiling
                    box.y = plat.bounds.y + plat.bounds.height;
                    velocity.y = 0.0f;
                    result.hitCeiling = true;
                    result.collidedY = true;
                }
            }
        }
    }
}

bool PhysicsWorld::CheckGrounded(const AABB& box, float skinWidth) const {
    AABB feetBox = box;
    feetBox.y = box.Bottom();
    feetBox.height = skinWidth;
    feetBox.x += 2.0f;
    feetBox.width -= 4.0f;

    for (const auto& plat : m_platforms) {
        if (plat.isHazard) continue;
        if (Intersects(feetBox, plat.bounds)) {
            // For one-way, only ground if feet are within tolerance of the top
            if (plat.isOneWay) {
                if (std::abs(box.Bottom() - plat.bounds.y) <= skinWidth + 1.0f) {
                    return true;
                }
            } else {
                return true;
            }
        }
    }
    return false;
}

void PhysicsWorld::DrawDebug(Color solidCol, Color oneWayCol, Color hazardCol) const {
    for (const auto& plat : m_platforms) {
        Rectangle r = plat.bounds.ToRectangle();
        if (plat.isHazard) {
            DrawRectangleLinesEx(r, 1.0f, hazardCol);
            DrawLine(r.x, r.y + r.height, r.x + r.width, r.y, hazardCol);
        } else if (plat.isOneWay) {
            DrawRectangleLinesEx(r, 1.0f, oneWayCol);
            DrawLine(r.x, r.y, r.x + r.width, r.y, WHITE); // Highlight top edge
        } else {
            DrawRectangleLinesEx(r, 1.0f, solidCol);
        }
    }
}

} // namespace riftbound

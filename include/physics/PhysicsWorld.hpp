#pragma once

#include "physics/AABB.hpp"
#include "physics/Collider.hpp"
#include <vector>

namespace riftbound {

class PhysicsWorld {
public:
    PhysicsWorld() = default;

    void Clear();
    void AddSolid(AABB box);
    void AddOneWayPlatform(AABB box);
    void AddHazard(AABB box, float damage = 25.0f);

    const std::vector<PlatformCollider>& GetPlatforms() const { return m_platforms; }

    // Moves an AABB using axis-separated collision resolution against level solids & one-way platforms.
    // Handles tunneling prevention via sub-stepping.
    CollisionResult MoveAndSlide(AABB& box, Vector2& velocity, float dt, bool dropThroughOneWay = false);

    // Tests if an AABB is standing on ground
    bool CheckGrounded(const AABB& box, float skinWidth = 2.0f) const;

    // Debug rendering of all collision boxes
    void DrawDebug(Color solidCol = RED, Color oneWayCol = SKYBLUE, Color hazardCol = ORANGE) const;

private:
    std::vector<PlatformCollider> m_platforms;

    void ResolveAxisX(AABB& box, Vector2& velocity, float stepX, CollisionResult& result);
    void ResolveAxisY(AABB& box, Vector2& velocity, float stepY, bool dropThroughOneWay, CollisionResult& result);
};

} // namespace riftbound

#include "physics/AABB.hpp"
#include "physics/PhysicsWorld.hpp"
#include <iostream>
#include <cassert>

extern int g_totalTests;
extern int g_passedTests;

#define EXPECT_TRUE(cond) do { \
    g_totalTests++; \
    if (cond) { \
        g_passedTests++; \
    } else { \
        std::cerr << "FAILED: " << #cond << " at " << __FILE__ << ":" << __LINE__ << "\n"; \
    } \
} while (0)

#define EXPECT_FALSE(cond) EXPECT_TRUE(!(cond))

void RunPhysicsTests() {
    std::cout << "[Physics] Running AABB & Collision tests...\n";

    using namespace riftbound;

    // Test 1: AABB Overlap
    {
        AABB boxA{ 10.0f, 10.0f, 20.0f, 20.0f };
        AABB boxB{ 25.0f, 25.0f, 20.0f, 20.0f }; // Overlaps
        AABB boxC{ 40.0f, 40.0f, 20.0f, 20.0f }; // Outside

        EXPECT_TRUE(Intersects(boxA, boxB));
        EXPECT_FALSE(Intersects(boxA, boxC));
    }

    // Test 2: Point containment
    {
        AABB box{ 0.0f, 0.0f, 50.0f, 50.0f };
        EXPECT_TRUE(Contains(box, Vector2{ 25.0f, 25.0f }));
        EXPECT_TRUE(Contains(box, Vector2{ 0.0f, 0.0f }));
        EXPECT_FALSE(Contains(box, Vector2{ 51.0f, 25.0f }));
    }

    // Test 3: Floor Collision Resolution
    {
        PhysicsWorld world;
        world.AddSolid(AABB{ 0.0f, 100.0f, 200.0f, 20.0f }); // Floor at y=100

        AABB player{ 50.0f, 70.0f, 20.0f, 20.0f }; // Bottom at y=90
        Vector2 vel{ 0.0f, 400.0f }; // High falling speed
        float dt = 0.05f; // would move 20px, penetrating to y=110

        CollisionResult res = world.MoveAndSlide(player, vel, dt);
        EXPECT_TRUE(res.grounded);
        EXPECT_TRUE(res.collidedY);
        EXPECT_TRUE(player.y == 80.0f); // Snapped exactly on top of floor (100 - 20)
        EXPECT_TRUE(vel.y == 0.0f);
    }

    // Test 4: Wall Collision Resolution
    {
        PhysicsWorld world;
        world.AddSolid(AABB{ 100.0f, 0.0f, 20.0f, 100.0f }); // Wall at x=100

        AABB player{ 75.0f, 20.0f, 20.0f, 20.0f }; // Right at x=95
        Vector2 vel{ 200.0f, 0.0f }; // Moving right
        float dt = 0.05f; // would move 10px to x=85..105, penetrating wall

        CollisionResult res = world.MoveAndSlide(player, vel, dt);
        EXPECT_TRUE(res.collidedX);
        EXPECT_TRUE(player.x == 80.0f); // Snapped to wall left edge (100 - 20)
        EXPECT_TRUE(vel.x == 0.0f);
    }

    // Test 5: One-way Platform: pass through upwards
    {
        PhysicsWorld world;
        world.AddOneWayPlatform(AABB{ 0.0f, 100.0f, 100.0f, 10.0f });

        AABB player{ 20.0f, 120.0f, 20.0f, 20.0f }; // Below platform
        Vector2 vel{ 0.0f, -300.0f }; // Jumping upwards
        float dt = 0.05f;

        CollisionResult res = world.MoveAndSlide(player, vel, dt);
        EXPECT_FALSE(res.grounded);
        EXPECT_FALSE(res.collidedY);
        EXPECT_TRUE(player.y < 120.0f); // Moved upward freely through one-way ledge
    }

    // Test 6: One-way Platform: land on top when falling
    {
        PhysicsWorld world;
        world.AddOneWayPlatform(AABB{ 0.0f, 100.0f, 100.0f, 10.0f });

        AABB player{ 20.0f, 75.0f, 20.0f, 20.0f }; // Bottom at y=95 (above platform at y=100)
        Vector2 vel{ 0.0f, 200.0f }; // Falling
        float dt = 0.05f; // would penetrate

        CollisionResult res = world.MoveAndSlide(player, vel, dt);
        EXPECT_TRUE(res.grounded);
        EXPECT_TRUE(player.y == 80.0f); // Landed exactly on top (100 - 20)
    }

    std::cout << "[Physics] Tests completed.\n";
}

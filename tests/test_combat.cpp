#include "combat/EmberBlade.hpp"
#include "combat/Projectile.hpp"
#include "entities/Patroller.hpp"
#include "entities/Boss.hpp"
#include <iostream>

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

void RunCombatTests() {
    std::cout << "[Combat] Running weapons, damage & entity tests...\n";

    using namespace riftbound;

    // Test 1: Ember Blade single hit prevention
    {
        EmberBlade blade;
        EXPECT_TRUE(blade.CanAttack());
        blade.Attack(Vector2{ 100.0f, 100.0f }, Direction::Right);
        EXPECT_FALSE(blade.CanAttack()); // On cooldown

        uint32_t enemyId = 42;
        EXPECT_FALSE(blade.HasHitEntity(enemyId));
        blade.RegisterHitEntity(enemyId);
        EXPECT_TRUE(blade.HasHitEntity(enemyId)); // Repeated hit blocked during same swing
    }

    // Test 2: Projectile lifetime expiration
    {
        Projectile proj;
        proj.Spawn(Vector2{ 50.0f, 50.0f }, Vector2{ 100.0f, 0.0f }, 20.0f, 1.0f, 5.0f, true);
        EXPECT_TRUE(proj.IsActive());

        proj.Update(0.5f);
        EXPECT_TRUE(proj.IsActive());
        EXPECT_TRUE(proj.GetPosition().x == 100.0f);

        proj.Update(0.6f); // Total 1.1s > 1.0s max lifetime
        EXPECT_FALSE(proj.IsActive()); // Expired
    }

    // Test 3: Enemy damage & invulnerability
    {
        Patroller patroller(Vector2{ 200.0f, 200.0f }, 100.0f, 300.0f);
        float initHp = patroller.GetHealth();

        DamageInfo dmg;
        dmg.amount = 20.0f;
        dmg.attackId = 1;
        bool hit1 = patroller.TakeDamage(dmg);
        EXPECT_TRUE(hit1);
        EXPECT_TRUE(patroller.GetHealth() == initHp - 20.0f);

        // Immediate second hit with same attackId should be ignored
        bool hit2 = patroller.TakeDamage(dmg);
        EXPECT_FALSE(hit2);
        EXPECT_TRUE(patroller.GetHealth() == initHp - 20.0f);
    }

    // Test 4: Boss Phase Transitions
    {
        AshenWarden warden(Vector2{ 500.0f, 500.0f }, 200.0f, 800.0f);
        warden.ActivateEncounter();
        EXPECT_TRUE(warden.GetPhase() == BossPhase::Phase1);

        // Deal damage down to 60% HP -> enters Phase 2
        DamageInfo d1;
        d1.amount = 160.0f;
        d1.attackId = 10;
        warden.TakeDamage(d1);
        EXPECT_TRUE(warden.GetPhase() == BossPhase::Phase2);

        // Deal damage down to 25% HP -> enters Phase 3
        DamageInfo d2;
        d2.amount = 150.0f;
        d2.attackId = 11;
        warden.TakeDamage(d2);
        EXPECT_TRUE(warden.GetPhase() == BossPhase::Phase3);

        // Deal fatal damage
        DamageInfo d3;
        d3.amount = 150.0f;
        d3.attackId = 12;
        warden.TakeDamage(d3);
        EXPECT_TRUE(warden.IsDefeated());
    }

    std::cout << "[Combat] Tests completed.\n";
}

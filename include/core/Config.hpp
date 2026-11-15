#pragma once

#include <raylib.h>

namespace riftbound {

struct DisplayConfig {
    int virtualWidth = 640;
    int virtualHeight = 360;
    int windowWidth = 1280;
    int windowHeight = 720;
    int targetFps = 60;
    const char* windowTitle = "Riftbound: Echoes of Ember";
};

struct PhysicsConfig {
    float gravity = 920.0f;
    float maxFallSpeed = 580.0f;
    float fixedDeltaTime = 1.0f / 60.0f;
};

struct PlayerConfig {
    float moveSpeed = 190.0f;
    float acceleration = 1300.0f;
    float deceleration = 1600.0f;
    float airAcceleration = 900.0f;
    float airDeceleration = 600.0f;
    float jumpVelocity = -380.0f;
    float minJumpVelocity = -150.0f; // For variable jump height on early release
    float coyoteTimeDuration = 0.12f;
    float jumpBufferDuration = 0.12f;
    float maxHealth = 100.0f;
    float invulnerabilityDuration = 1.2f;
    float knockbackForceX = 180.0f;
    float knockbackForceY = -160.0f;
    float width = 18.0f;
    float height = 30.0f;
};

struct WeaponConfig {
    // Ember Blade (Melee)
    float bladeDamage = 40.0f;
    float bladeCooldown = 0.28f;
    float bladeReachX = 36.0f;
    float bladeReachY = 32.0f;
    float bladeKnockback = 200.0f;

    // Pulse Bolts (Ranged)
    float boltDamage = 22.0f;
    float boltSpeed = 460.0f;
    float boltLifetime = 1.6f;
    float boltCooldown = 0.20f;
    float boltEnergyCost = 14.0f;
    float maxEnergy = 100.0f;
    float energyRegenRate = 15.0f; // per second
    float boltRadius = 4.5f;
};

struct EnemyConfig {
    // Patroller
    float patrollerHealth = 45.0f;
    float patrollerSpeed = 55.0f;
    float patrollerDamage = 20.0f;
    float patrollerWidth = 22.0f;
    float patrollerHeight = 26.0f;

    // Ranged Sentry
    float sentryHealth = 35.0f;
    float sentryFireInterval = 2.2f;
    float sentryProjectileSpeed = 210.0f;
    float sentryDamage = 18.0f;
    float sentryWidth = 24.0f;
    float sentryHeight = 28.0f;

    // Chaser
    float chaserHealth = 55.0f;
    float chaserSpeed = 110.0f;
    float chaserAggroRange = 190.0f;
    float chaserDamage = 25.0f;
    float chaserWidth = 20.0f;
    float chaserHeight = 28.0f;

    // Boss - The Ashen Warden
    float bossMaxHealth = 400.0f;
    float bossWidth = 48.0f;
    float bossHeight = 64.0f;
    float bossPhase2HealthPct = 0.66f;
    float bossPhase3HealthPct = 0.33f;
    float bossSlamCooldown = 3.5f;
    float bossVolleyCooldown = 3.0f;
    float bossChargeCooldown = 4.0f;
    float bossSlamDamage = 35.0f;
    float bossProjectileDamage = 20.0f;
    float bossChargeDamage = 40.0f;
};

struct GameConfig {
    DisplayConfig display;
    PhysicsConfig physics;
    PlayerConfig player;
    WeaponConfig weapons;
    EnemyConfig enemies;

    // Settings
    float masterVolume = 0.8f;
    float musicVolume = 0.6f;
    float sfxVolume = 0.8f;
    bool screenShakeEnabled = true;
    bool debugHitboxes = false;
};

// Global config access
GameConfig& GetGameConfig();

} // namespace riftbound

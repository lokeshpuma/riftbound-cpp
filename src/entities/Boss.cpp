#include "entities/Boss.hpp"
#include "core/Config.hpp"
#include "core/AssetManager.hpp"
#include "core/AudioPlayer.hpp"
#include "core/ParticleSystem.hpp"
#include "physics/PhysicsWorld.hpp"
#include <cmath>
#include <cstdlib>
#include <iostream>

namespace riftbound {

AshenWarden::AshenWarden(Vector2 arenaCenter, float arenaLeft, float arenaRight)
    : Enemy(EntityType::Boss, arenaCenter, Vector2{ 48.0f, 64.0f }, 400.0f, 30.0f),
      m_arenaLeft(arenaLeft),
      m_arenaRight(arenaRight),
      m_spawnPos(arenaCenter) {
    const auto& cfg = GetGameConfig();
    m_health = cfg.enemies.bossMaxHealth;
    m_maxHealth = cfg.enemies.bossMaxHealth;
    m_size = { cfg.enemies.bossWidth, cfg.enemies.bossHeight };
    m_contactDamage = 30.0f;
    m_facing = Direction::Left;
}

void AshenWarden::Reset() {
    m_position = m_spawnPos;
    m_velocity = { 0.0f, 0.0f };
    m_health = m_maxHealth;
    m_isAlive = true;
    m_encounterActive = false;
    m_phase = BossPhase::Phase1;
    m_state = BossState::Idle;
    m_stateTimer = 0.0f;
    m_attackCooldown = 2.0f;
    m_hurtFlashTimer = 0.0f;
    m_invulnTimer = 0.0f;
}

bool AshenWarden::TakeDamage(const DamageInfo& info) {
    if (!m_encounterActive || !m_isAlive) return false;

    // Boss cannot take damage while airborne in high jump slam
    if (m_state == BossState::TelegraphSlam || m_state == BossState::Defeated) {
        return false;
    }

    bool tookDmg = Enemy::TakeDamage(info);
    if (tookDmg) {
        // Update phase based on health percentage
        float hpPct = m_health / m_maxHealth;
        const auto& cfg = GetGameConfig();

        if (hpPct <= cfg.enemies.bossPhase3HealthPct && m_phase != BossPhase::Phase3) {
            m_phase = BossPhase::Phase3;
            AudioPlayer::Instance().PlaySoundEffect(SoundId::BossRoar);
            ParticleSystem::Instance().BurstBossSlam(m_position, 80.0f);
        } else if (hpPct <= cfg.enemies.bossPhase2HealthPct && m_phase == BossPhase::Phase1) {
            m_phase = BossPhase::Phase2;
            AudioPlayer::Instance().PlaySoundEffect(SoundId::BossRoar);
            ParticleSystem::Instance().BurstBossSlam(m_position, 60.0f);
        }

        if (!m_isAlive) {
            m_state = BossState::Defeated;
            m_stateTimer = 0.0f;
            AudioPlayer::Instance().PlaySoundEffect(SoundId::BossRoar);
            AudioPlayer::Instance().PlaySoundEffect(SoundId::Victory);
            ParticleSystem::Instance().BurstBossSlam(m_position, 120.0f);
        }
    }
    return tookDmg;
}

void AshenWarden::Update(float dt, PhysicsWorld& world) {
    if (!m_encounterActive) return;

    if (m_hurtFlashTimer > 0.0f) m_hurtFlashTimer -= dt;
    if (m_invulnTimer > 0.0f) m_invulnTimer -= dt;

    m_animTimer += dt;
    const auto& cfg = GetGameConfig();

    if (m_state == BossState::Defeated) {
        // Dramatic death sequence
        m_stateTimer += dt;
        m_velocity = { 0.0f, 0.0f };
        if (m_stateTimer < 2.0f) {
            ParticleSystem::Instance().BurstEmberSparks(m_position, Vector2{ 0.0f, -1.0f }, 4);
        }
        return;
    }

    // Facing direction
    if (m_state != BossState::PerformCharge && m_state != BossState::TelegraphCharge) {
        m_facing = (m_targetPos.x < m_position.x) ? Direction::Left : Direction::Right;
    }

    // State machine logic
    switch (m_state) {
        case BossState::Idle: {
            m_velocity.x *= (1.0f - 8.0f * dt);
            m_attackCooldown -= dt;
            if (m_attackCooldown <= 0.0f) {
                ChooseNextAttack();
            }
            break;
        }

        case BossState::TelegraphSlam: {
            // Jump high up out of view, telegraph zone appears on ground
            m_stateTimer += dt;
            m_velocity.x = 0.0f;
            m_velocity.y = -400.0f; // Rapid ascent

            // Particle embers indicating impending strike
            Vector2 floorTarget = { m_slamTargetPos.x, m_spawnPos.y + 16.0f };
            ParticleSystem::Instance().Emit(
                floorTarget,
                Vector2{ (((float)rand() / RAND_MAX) - 0.5f) * 60.0f, -40.0f },
                Color{ 255, 60, 20, 255 },
                Color{ 255, 180, 0, 0 },
                4.0f, 0.3f
            );

            if (m_stateTimer >= 1.0f) {
                // Drop straight down on target!
                m_state = BossState::PerformSlam;
                m_position.x = m_slamTargetPos.x;
                m_position.y = m_spawnPos.y - 200.0f;
                m_velocity.y = 800.0f; // Meteoric slam downward
                m_stateTimer = 0.0f;
            }
            break;
        }

        case BossState::PerformSlam: {
            m_velocity.y += 1200.0f * dt;
            AABB box = GetBounds();
            CollisionResult col = world.MoveAndSlide(box, m_velocity, dt);
            m_position.x = box.x + box.width * 0.5f;
            m_position.y = box.y + box.height * 0.5f;

            if (col.grounded) {
                ExecuteGroundSlam();
                m_state = BossState::Recovery;
                m_stateTimer = (m_phase == BossPhase::Phase3) ? 0.9f : 1.4f; // Vulnerability window
            }
            break;
        }

        case BossState::TelegraphVolley: {
            m_stateTimer += dt;
            m_velocity = { 0.0f, 0.0f };

            // Embers gather into boss chest
            ParticleSystem::Instance().Emit(
                m_position,
                Vector2{ (((float)rand() / RAND_MAX) - 0.5f) * 80.0f, (((float)rand() / RAND_MAX) - 0.5f) * 80.0f },
                Color{ 255, 180, 40, 255 },
                Color{ 255, 50, 0, 0 },
                4.0f, 0.25f
            );

            if (m_stateTimer >= 0.75f) {
                ExecuteVolley();
                m_state = BossState::Recovery;
                m_stateTimer = (m_phase == BossPhase::Phase3) ? 0.8f : 1.3f;
            }
            break;
        }

        case BossState::TelegraphCharge: {
            m_stateTimer += dt;
            m_velocity = { 0.0f, 0.0f };

            // Burning rush stance
            float dirX = (m_chargeDir == Direction::Left) ? -1.0f : 1.0f;
            ParticleSystem::Instance().BurstEmberSparks(m_position, Vector2{ -dirX, 0.0f }, 3);

            if (m_stateTimer >= 0.6f) {
                m_state = BossState::PerformCharge;
                m_stateTimer = 0.0f;
                AudioPlayer::Instance().PlaySoundEffect(SoundId::BossRoar);
            }
            break;
        }

        case BossState::PerformCharge: {
            ExecuteCharge(dt, world);
            break;
        }

        case BossState::Recovery: {
            // Boss breathes heavily / cooldown window where player can safely attack!
            m_velocity.x *= (1.0f - 8.0f * dt);
            m_stateTimer -= dt;
            if (m_stateTimer <= 0.0f) {
                m_state = BossState::Idle;
                m_attackCooldown = (m_phase == BossPhase::Phase3) ? 1.0f : 1.8f;
            }
            break;
        }

        default:
            break;
    }

    // Normal gravity integration when not jumping slam
    if (m_state != BossState::TelegraphSlam && m_state != BossState::PerformSlam) {
        m_velocity.y += cfg.physics.gravity * dt;
        if (m_velocity.y > cfg.physics.maxFallSpeed) {
            m_velocity.y = cfg.physics.maxFallSpeed;
        }

        AABB box = GetBounds();
        world.MoveAndSlide(box, m_velocity, dt);
        m_position.x = box.x + box.width * 0.5f;
        m_position.y = box.y + box.height * 0.5f;
    }

    // Clamp boss strictly within arena boundaries
    if (m_position.x - m_size.x * 0.5f < m_arenaLeft) {
        m_position.x = m_arenaLeft + m_size.x * 0.5f;
        if (m_state == BossState::PerformCharge) {
            // Reached arena edge during charge, enter recovery
            m_state = BossState::Recovery;
            m_stateTimer = 1.2f;
            ParticleSystem::Instance().BurstBossSlam(m_position, 40.0f);
        }
    } else if (m_position.x + m_size.x * 0.5f > m_arenaRight) {
        m_position.x = m_arenaRight - m_size.x * 0.5f;
        if (m_state == BossState::PerformCharge) {
            m_state = BossState::Recovery;
            m_stateTimer = 1.2f;
            ParticleSystem::Instance().BurstBossSlam(m_position, 40.0f);
        }
    }
}

void AshenWarden::ChooseNextAttack() {
    int roll = rand() % 100;

    if (m_phase == BossPhase::Phase1) {
        // Phase 1: Heavy emphasis on Ground Slam & Volley
        if (roll < 60) {
            m_state = BossState::TelegraphSlam;
            m_stateTimer = 0.0f;
            m_slamTargetPos = m_targetPos;
        } else {
            m_state = BossState::TelegraphVolley;
            m_stateTimer = 0.0f;
        }
    } else if (m_phase == BossPhase::Phase2) {
        // Phase 2: Equal mix of Slam, Multi-volley, Charge
        if (roll < 40) {
            m_state = BossState::TelegraphVolley;
            m_stateTimer = 0.0f;
        } else if (roll < 75) {
            m_state = BossState::TelegraphSlam;
            m_stateTimer = 0.0f;
            m_slamTargetPos = m_targetPos;
        } else {
            m_state = BossState::TelegraphCharge;
            m_stateTimer = 0.0f;
            m_chargeDir = (m_targetPos.x < m_position.x) ? Direction::Left : Direction::Right;
        }
    } else {
        // Phase 3: Aggressive Charge and Slam attacks
        if (roll < 45) {
            m_state = BossState::TelegraphCharge;
            m_stateTimer = 0.0f;
            m_chargeDir = (m_targetPos.x < m_position.x) ? Direction::Left : Direction::Right;
        } else if (roll < 80) {
            m_state = BossState::TelegraphSlam;
            m_stateTimer = 0.0f;
            m_slamTargetPos = m_targetPos;
        } else {
            m_state = BossState::TelegraphVolley;
            m_stateTimer = 0.0f;
        }
    }
}

void AshenWarden::ExecuteGroundSlam() {
    AudioPlayer::Instance().PlaySoundEffect(SoundId::BossSlam);
    ParticleSystem::Instance().BurstBossSlam(m_position, m_slamWarningRadius);

    // Spawn 2 shockwave projectiles travelling along floor left and right
    if (m_projectileCallback) {
        Vector2 leftShock = { m_position.x - 24.0f, m_position.y + m_size.y * 0.5f - 8.0f };
        Vector2 rightShock = { m_position.x + 24.0f, m_position.y + m_size.y * 0.5f - 8.0f };
        m_projectileCallback(leftShock, Vector2{ -220.0f, 0.0f }, 25.0f, 1.8f, 7.0f, false);
        m_projectileCallback(rightShock, Vector2{ 220.0f, 0.0f }, 25.0f, 1.8f, 7.0f, false);
    }
}

void AshenWarden::ExecuteVolley() {
    AudioPlayer::Instance().PlaySoundEffect(SoundId::RangedFire);

    if (!m_projectileCallback) return;

    int projectileCount = (m_phase == BossPhase::Phase3) ? 5 : 3;
    float spreadAngle = 0.35f; // Radians

    float dx = m_targetPos.x - m_position.x;
    float dy = m_targetPos.y - m_position.y;
    float baseAngle = std::atan2(dy, dx);

    for (int i = 0; i < projectileCount; ++i) {
        float angleOffset = (i - (projectileCount - 1) * 0.5f) * spreadAngle;
        float finalAngle = baseAngle + angleOffset;
        float speed = 230.0f;
        Vector2 vel = { std::cos(finalAngle) * speed, std::sin(finalAngle) * speed };
        m_projectileCallback(m_position, vel, 20.0f, 3.0f, 6.0f, false);
    }
}

void AshenWarden::ExecuteCharge(float dt, PhysicsWorld& world) {
    float dirX = (m_chargeDir == Direction::Left) ? -1.0f : 1.0f;
    m_velocity.x = dirX * m_chargeSpeed;

    // Trail fire particles
    ParticleSystem::Instance().BurstEmberSparks(m_position, Vector2{ -dirX, 0.0f }, 3);

    m_stateTimer += dt;
    if (m_stateTimer >= 1.2f) {
        m_state = BossState::Recovery;
        m_stateTimer = 1.0f;
        m_velocity.x = 0.0f;
    }

    AABB box = GetBounds();
    CollisionResult col = world.MoveAndSlide(box, m_velocity, dt);
    m_position.x = box.x + box.width * 0.5f;
    m_position.y = box.y + box.height * 0.5f;

    if (col.collidedX) {
        m_state = BossState::Recovery;
        m_stateTimer = 1.2f;
        ParticleSystem::Instance().BurstBossSlam(m_position, 50.0f);
    }
}

void AshenWarden::Draw() const {
    if (!m_encounterActive) return;

    // Ground slam telegraph marker
    if (m_state == BossState::TelegraphSlam) {
        DrawCircleLines(static_cast<int>(m_slamTargetPos.x), static_cast<int>(m_spawnPos.y + 20.0f), m_slamWarningRadius, RED);
        DrawCircle(static_cast<int>(m_slamTargetPos.x), static_cast<int>(m_spawnPos.y + 20.0f), m_slamWarningRadius * 0.3f, Color{ 255, 50, 0, 100 });
        DrawText("WARNING: SLAM", static_cast<int>(m_slamTargetPos.x - 45.0f), static_cast<int>(m_spawnPos.y - 10.0f), 10, ORANGE);
    }

    Texture2D tex = AssetManager::Instance().GetTexture(TextureId::Boss);
    float dirX = (m_facing == Direction::Left) ? -1.0f : 1.0f;

    float pulseSize = 0.0f;
    if (m_state == BossState::TelegraphVolley || m_state == BossState::TelegraphCharge) {
        pulseSize = std::sin(m_animTimer * 20.0f) * 4.0f;
    }

    if (tex.id > 0) {
        Rectangle src = { 0, 0, (dirX < 0) ? -64.0f : 64.0f, 64.0f };
        Rectangle dest = { m_position.x, m_position.y, 64.0f + pulseSize, 64.0f + pulseSize };
        Vector2 origin = { dest.width * 0.5f, dest.height * 0.5f };
        Color tint = (m_hurtFlashTimer > 0.0f) ? Color{ 255, 100, 100, 255 } : WHITE;
        if (m_state == BossState::Recovery) {
            tint = Color{ 180, 180, 220, 255 }; // Tired / vulnerable visual tint
        }
        DrawTexturePro(tex, src, dest, origin, 0.0f, tint);
    } else {
        Color c = (m_hurtFlashTimer > 0.0f) ? WHITE : Color{ 80, 40, 90, 255 };
        DrawRectangle(static_cast<int>(m_position.x - m_size.x * 0.5f),
                      static_cast<int>(m_position.y - m_size.y * 0.5f),
                      static_cast<int>(m_size.x),
                      static_cast<int>(m_size.y), c);
    }

    // Vulnerable state indicator
    if (m_state == BossState::Recovery) {
        DrawText("VULNERABLE!", static_cast<int>(m_position.x - 38.0f), static_cast<int>(m_position.y - m_size.y * 0.5f - 14.0f), 10, GREEN);
    }

    if (GetGameConfig().debugHitboxes) {
        DrawRectangleLinesEx(GetBounds().ToRectangle(), 1.0f, MAGENTA);
    }
}

} // namespace riftbound

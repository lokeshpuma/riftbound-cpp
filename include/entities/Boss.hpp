#pragma once

#include "entities/Enemy.hpp"
#include <cstdint>

namespace riftbound {

enum class BossPhase : uint8_t {
    Phase1,
    Phase2,
    Phase3
};

enum class BossState : uint8_t {
    Intro,
    Idle,
    TelegraphSlam,
    PerformSlam,
    TelegraphVolley,
    PerformVolley,
    TelegraphCharge,
    PerformCharge,
    Recovery,
    Defeated
};

class AshenWarden : public Enemy {
public:
    AshenWarden(Vector2 arenaCenter, float arenaLeft, float arenaRight);

    void Update(float dt, PhysicsWorld& world) override;
    void Draw() const override;
    bool TakeDamage(const DamageInfo& info) override;
    void Reset();

    BossPhase GetPhase() const { return m_phase; }
    BossState GetBossState() const { return m_state; }
    bool IsDefeated() const { return m_state == BossState::Defeated; }
    bool IsEncounterActive() const { return m_encounterActive; }
    void ActivateEncounter() { m_encounterActive = true; }

    void SetTargetPosition(Vector2 target) { m_targetPos = target; }

private:
    float m_arenaLeft;
    float m_arenaRight;
    Vector2 m_spawnPos;
    Vector2 m_targetPos = { 0.0f, 0.0f };

    bool m_encounterActive = false;
    BossPhase m_phase = BossPhase::Phase1;
    BossState m_state = BossState::Idle;

    float m_stateTimer = 0.0f;
    float m_attackCooldown = 2.0f;
    float m_animTimer = 0.0f;

    // Slam attack data
    Vector2 m_slamTargetPos = { 0.0f, 0.0f };
    float m_slamWarningRadius = 70.0f;

    // Charge attack data
    float m_chargeSpeed = 340.0f;
    Direction m_chargeDir = Direction::Left;

    void ChooseNextAttack();
    void ExecuteGroundSlam();
    void ExecuteVolley();
    void ExecuteCharge(float dt, PhysicsWorld& world);
};

} // namespace riftbound

#pragma once

#include "entities/Entity.hpp"
#include "combat/EmberBlade.hpp"
#include "combat/PulseBolts.hpp"
#include <memory>
#include <vector>

namespace riftbound {

enum class PlayerAnimState {
    Idle,
    Run,
    Jump,
    Fall,
    Attack,
    Hurt,
    Dead
};

class Player : public Entity {
public:
    Player(Vector2 spawnPos);

    void Update(float dt, PhysicsWorld& world) override;
    void Draw() const override;
    bool TakeDamage(const DamageInfo& info) override;

    // Movement and combat actions
    void HandleInput();
    void Jump();
    void ReleaseJump();
    void AttackMelee();
    void AttackRanged();
    void SwitchWeapon();

    // Checkpoint & Respawn
    void SetCheckpoint(Vector2 cpPos);
    void RespawnAtCheckpoint();

    // Stats & Inventory
    void Heal(float amount);
    void RestoreEnergy(float amount);
    float GetEnergy() const { return m_energy; }
    float GetMaxEnergy() const { return m_maxEnergy; }
    WeaponType GetCurrentWeaponType() const { return m_currentWeapon; }
    Weapon* GetActiveWeapon();
    EmberBlade& GetEmberBlade() { return m_blade; }
    PulseBolts& GetPulseBolts() { return m_pulseBolts; }

    bool IsGrounded() const { return m_isGrounded; }
    PlayerAnimState GetAnimState() const { return m_animState; }

private:
    // Movement mechanics
    bool m_isGrounded = false;
    float m_coyoteTimer = 0.0f;
    float m_jumpBufferTimer = 0.0f;
    bool m_jumpHeld = false;
    bool m_dropThroughOneWay = false;

    // Energy / ammo
    float m_energy = 100.0f;
    float m_maxEnergy = 100.0f;
    float m_energyRegenRate = 15.0f;

    // Weapons
    EmberBlade m_blade;
    PulseBolts m_pulseBolts;
    WeaponType m_currentWeapon = WeaponType::EmberBlade;

    // Respawn
    Vector2 m_checkpointPos;

    // Animation & Feedback
    PlayerAnimState m_animState = PlayerAnimState::Idle;
    float m_animTimer = 0.0f;
    float m_hurtTimer = 0.0f;
};

} // namespace riftbound

#include "entities/Player.hpp"
#include "core/Config.hpp"
#include "core/InputManager.hpp"
#include "core/AudioPlayer.hpp"
#include "core/ParticleSystem.hpp"
#include "core/AssetManager.hpp"
#include "physics/PhysicsWorld.hpp"
#include <cmath>
#include <algorithm>

namespace riftbound {

Player::Player(Vector2 spawnPos)
    : Entity(EntityType::Player, spawnPos, Vector2{ 18.0f, 30.0f }, 100.0f),
      m_checkpointPos(spawnPos) {
    const auto& cfg = GetGameConfig();
    m_health = cfg.player.maxHealth;
    m_maxHealth = cfg.player.maxHealth;
    m_size = { cfg.player.width, cfg.player.height };
    m_invulnDuration = cfg.player.invulnerabilityDuration;

    m_energy = cfg.weapons.maxEnergy;
    m_maxEnergy = cfg.weapons.maxEnergy;
    m_energyRegenRate = cfg.weapons.energyRegenRate;
}

void Player::SetCheckpoint(Vector2 cpPos) {
    m_checkpointPos = cpPos;
}

void Player::RespawnAtCheckpoint() {
    m_position = m_checkpointPos;
    m_velocity = { 0.0f, 0.0f };
    m_health = m_maxHealth;
    m_energy = m_maxEnergy;
    m_isAlive = true;
    m_invulnTimer = 0.5f;
    m_hurtTimer = 0.0f;
    m_animState = PlayerAnimState::Idle;
}

void Player::Heal(float amount) {
    m_health = std::min(m_maxHealth, m_health + amount);
}

void Player::RestoreEnergy(float amount) {
    m_energy = std::min(m_maxEnergy, m_energy + amount);
}

void Player::Kill() {
    m_health = 0.0f;
    m_isAlive = false;
    m_animState = PlayerAnimState::Dead;
}

Weapon* Player::GetActiveWeapon() {
    if (m_currentWeapon == WeaponType::EmberBlade) {
        return &m_blade;
    }
    return &m_pulseBolts;
}

void Player::HandleInput() {
    if (!m_isAlive) return;

    const auto& input = InputManager::Instance();
    const auto& cfg = GetGameConfig();

    float moveX = input.GetHorizontalAxis();
    bool wantsDown = input.IsDown(GameAction::MoveDown);

    // Drop through one-way platforms if holding down + jump
    if (wantsDown && input.IsPressed(GameAction::Jump)) {
        m_dropThroughOneWay = true;
    }

    if (input.IsPressed(GameAction::Jump)) {
        m_jumpBufferTimer = cfg.player.jumpBufferDuration;
    }
    m_jumpHeld = input.IsDown(GameAction::Jump);

    if (input.IsReleased(GameAction::Jump)) {
        ReleaseJump();
    }

    // Facing direction
    if (moveX > 0.1f) {
        m_facing = Direction::Right;
    } else if (moveX < -0.1f) {
        m_facing = Direction::Left;
    }

    // Attacks
    if (input.IsPressed(GameAction::MeleeAttack)) {
        AttackMelee();
    }
    if (input.IsPressed(GameAction::RangedAttack)) {
        AttackRanged();
    }

    // Weapon Switching
    if (input.IsPressed(GameAction::SwitchWeapon)) {
        SwitchWeapon();
    }
}

void Player::Jump() {
    const auto& cfg = GetGameConfig();
    m_velocity.y = cfg.player.jumpVelocity;
    m_isGrounded = false;
    m_coyoteTimer = 0.0f;
    m_jumpBufferTimer = 0.0f;

    AudioPlayer::Instance().PlaySoundEffect(SoundId::Jump);
    ParticleSystem::Instance().BurstDust(Vector2{ m_position.x, m_position.y + m_size.y * 0.5f }, 6);
}

void Player::ReleaseJump() {
    const auto& cfg = GetGameConfig();
    // Variable jump height: cut vertical upward velocity when releasing jump button early
    if (m_velocity.y < cfg.player.minJumpVelocity) {
        m_velocity.y = cfg.player.minJumpVelocity;
    }
}

void Player::AttackMelee() {
    m_currentWeapon = WeaponType::EmberBlade;
    m_blade.Attack(m_position, m_facing);
}

void Player::AttackRanged() {
    m_currentWeapon = WeaponType::PulseBolts;
    float cost = m_pulseBolts.GetEnergyCost();
    if (m_energy >= cost) {
        if (m_pulseBolts.Attack(m_position, m_facing)) {
            m_energy -= cost;
        }
    }
}

void Player::SwitchWeapon() {
    if (m_currentWeapon == WeaponType::EmberBlade) {
        m_currentWeapon = WeaponType::PulseBolts;
    } else {
        m_currentWeapon = WeaponType::EmberBlade;
    }
}

bool Player::TakeDamage(const DamageInfo& info) {
    if (!m_isAlive || m_invulnTimer > 0.0f) return false;

    bool tookDmg = Entity::TakeDamage(info);
    if (tookDmg) {
        m_hurtTimer = 0.25f;
        AudioPlayer::Instance().PlaySoundEffect(SoundId::PlayerHurt);
        ParticleSystem::Instance().BurstImpact(m_position, Color{ 255, 60, 60, 255 }, 12);
        ParticleSystem::Instance().TriggerScreenShake(5.0f, 0.25f);
        if (!m_isAlive) {
            AudioPlayer::Instance().PlaySoundEffect(SoundId::GameOver);
        }
    }
    return tookDmg;
}

void Player::Update(float dt, PhysicsWorld& world) {
    if (!m_isAlive) {
        m_animState = PlayerAnimState::Dead;
        return;
    }

    const auto& cfg = GetGameConfig();
    const auto& input = InputManager::Instance();

    // Weapon updates
    m_blade.Update(dt);
    m_pulseBolts.Update(dt);

    // Energy regeneration
    if (m_energy < m_maxEnergy) {
        m_energy = std::min(m_maxEnergy, m_energy + m_energyRegenRate * dt);
    }

    // Timers
    if (m_invulnTimer > 0.0f) m_invulnTimer -= dt;
    if (m_hurtTimer > 0.0f) m_hurtTimer -= dt;

    // Coyote and buffer timers
    if (m_isGrounded) {
        m_coyoteTimer = cfg.player.coyoteTimeDuration;
    } else {
        m_coyoteTimer = std::max(0.0f, m_coyoteTimer - dt);
    }

    if (m_jumpBufferTimer > 0.0f) {
        m_jumpBufferTimer -= dt;
    }

    // Execute buffered or coyote jump
    if (m_jumpBufferTimer > 0.0f && m_coyoteTimer > 0.0f) {
        Jump();
    }

    // Horizontal movement integration with acceleration / deceleration
    float targetSpeed = input.GetHorizontalAxis() * cfg.player.moveSpeed;
    float accel = m_isGrounded ? cfg.player.acceleration : cfg.player.airAcceleration;
    float decel = m_isGrounded ? cfg.player.deceleration : cfg.player.airDeceleration;

    if (std::abs(targetSpeed) > 0.01f) {
        // Accelerating
        if (targetSpeed > m_velocity.x) {
            m_velocity.x = std::min(targetSpeed, m_velocity.x + accel * dt);
        } else if (targetSpeed < m_velocity.x) {
            m_velocity.x = std::max(targetSpeed, m_velocity.x - accel * dt);
        }
    } else {
        // Decelerating to 0
        if (m_velocity.x > 0.0f) {
            m_velocity.x = std::max(0.0f, m_velocity.x - decel * dt);
        } else if (m_velocity.x < 0.0f) {
            m_velocity.x = std::min(0.0f, m_velocity.x + decel * dt);
        }
    }

    // Apply gravity
    m_velocity.y += cfg.physics.gravity * dt;
    if (m_velocity.y > cfg.physics.maxFallSpeed) {
        m_velocity.y = cfg.physics.maxFallSpeed;
    }

    // Physics movement and collision resolution
    AABB box = GetBounds();
    bool wasGrounded = m_isGrounded;
    CollisionResult col = world.MoveAndSlide(box, m_velocity, dt, m_dropThroughOneWay);

    // Update position from resolved AABB box
    m_position.x = box.x + box.width * 0.5f;
    m_position.y = box.y + box.height * 0.5f;

    m_isGrounded = col.grounded;
    m_dropThroughOneWay = false;

    // Landing feedback
    if (!wasGrounded && m_isGrounded) {
        ParticleSystem::Instance().BurstDust(Vector2{ m_position.x, m_position.y + m_size.y * 0.5f }, 5);
    }

    // Animation state selection
    if (m_hurtTimer > 0.0f) {
        m_animState = PlayerAnimState::Hurt;
    } else if (m_blade.IsSlashing()) {
        m_animState = PlayerAnimState::Attack;
    } else if (!m_isGrounded) {
        m_animState = (m_velocity.y < 0.0f) ? PlayerAnimState::Jump : PlayerAnimState::Fall;
    } else if (std::abs(m_velocity.x) > 10.0f) {
        m_animState = PlayerAnimState::Run;
    } else {
        m_animState = PlayerAnimState::Idle;
    }

    m_animTimer += dt;
}

void Player::Draw() const {
    if (!m_isAlive) {
        // Draw defeated hero body resting on ground with embers
        Texture2D tex = AssetManager::Instance().GetTexture(TextureId::Player);
        float dirX = (m_facing == Direction::Left) ? -1.0f : 1.0f;
        if (tex.id > 0) {
            Rectangle src = { 0, 0, (dirX < 0) ? -32.0f : 32.0f, 32.0f };
            Rectangle dest = { m_position.x, m_position.y + 6.0f, 32.0f, 32.0f };
            Vector2 origin = { 16.0f, 16.0f };
            DrawTexturePro(tex, src, dest, origin, 90.0f * dirX, Color{ 180, 60, 60, 220 });
        } else {
            Rectangle r = { m_position.x - m_size.y * 0.5f, m_position.y + m_size.y * 0.2f, m_size.y, m_size.x * 0.8f };
            DrawRectangleRec(r, Color{ 140, 40, 40, 220 });
        }
        return;
    }

    // Invulnerability blink: modulate alpha/visibility every few frames
    if (m_invulnTimer > 0.0f) {
        int blinkCycle = static_cast<int>(m_invulnTimer * 20.0f);
        if (blinkCycle % 2 == 0) {
            return; // Skip draw for blink effect
        }
    }

    Texture2D tex = AssetManager::Instance().GetTexture(TextureId::Player);
    float dirX = (m_facing == Direction::Left) ? -1.0f : 1.0f;

    // Small bounce/tilt based on running
    float offsetY = 0.0f;
    if (m_animState == PlayerAnimState::Run) {
        offsetY = std::sin(m_animTimer * 16.0f) * 2.0f;
    }

    if (tex.id > 0) {
        Rectangle src = { 0, 0, (dirX < 0) ? -32.0f : 32.0f, 32.0f };
        Rectangle dest = { m_position.x, m_position.y + offsetY, 32.0f, 32.0f };
        Vector2 origin = { 16.0f, 16.0f };
        Color tint = (m_hurtTimer > 0.0f) ? Color{ 255, 120, 120, 255 } : WHITE;
        DrawTexturePro(tex, src, dest, origin, 0.0f, tint);
    } else {
        // Fallback rendering
        Color bodyCol = (m_hurtTimer > 0.0f) ? RED : Color{ 50, 70, 160, 255 };
        Rectangle r = { m_position.x - m_size.x * 0.5f, m_position.y - m_size.y * 0.5f + offsetY, m_size.x, m_size.y };
        DrawRectangleRec(r, bodyCol);
        // Visor
        float eyeX = (dirX > 0) ? (m_position.x + 2.0f) : (m_position.x - 6.0f);
        DrawRectangle(static_cast<int>(eyeX), static_cast<int>(m_position.y - 8.0f), 4, 3, GOLD);
    }

    // Render active weapon swing
    m_blade.Draw(m_position, m_facing);

    // Debug hitboxes
    if (GetGameConfig().debugHitboxes) {
        DrawRectangleLinesEx(GetBounds().ToRectangle(), 1.0f, GREEN);
    }
}

} // namespace riftbound

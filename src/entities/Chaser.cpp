#include "entities/Chaser.hpp"
#include "core/Config.hpp"
#include "core/AssetManager.hpp"
#include "core/ParticleSystem.hpp"
#include "physics/PhysicsWorld.hpp"
#include <cmath>

namespace riftbound {

Chaser::Chaser(Vector2 pos, float aggroRange)
    : Enemy(EntityType::Chaser, pos, Vector2{ 20.0f, 28.0f }, 55.0f, 25.0f),
      m_aggroRange(aggroRange) {
    const auto& cfg = GetGameConfig();
    m_health = cfg.enemies.chaserHealth;
    m_maxHealth = cfg.enemies.chaserHealth;
    m_speed = cfg.enemies.chaserSpeed;
    m_contactDamage = cfg.enemies.chaserDamage;
    m_size = { cfg.enemies.chaserWidth, cfg.enemies.chaserHeight };
}

void Chaser::Update(float dt, PhysicsWorld& world) {
    if (!m_isAlive) return;

    if (m_hurtFlashTimer > 0.0f) m_hurtFlashTimer -= dt;
    if (m_invulnTimer > 0.0f) m_invulnTimer -= dt;
    if (m_jumpCooldown > 0.0f) m_jumpCooldown -= dt;

    const auto& cfg = GetGameConfig();

    float dx = m_targetPos.x - m_position.x;
    float dy = m_targetPos.y - m_position.y;
    float dist = std::sqrt(dx * dx + dy * dy);

    m_isChasing = (dist <= m_aggroRange);

    if (m_isChasing) {
        float dirX = (dx > 0.0f) ? 1.0f : -1.0f;
        m_facing = (dirX > 0.0f) ? Direction::Right : Direction::Left;
        m_velocity.x = dirX * m_speed;

        // Jump over small obstacles or towards player if player is higher
        if (m_jumpCooldown <= 0.0f && world.CheckGrounded(GetBounds())) {
            if (dy < -20.0f || std::abs(m_velocity.x) < 10.0f) {
                m_velocity.y = -260.0f;
                m_jumpCooldown = 1.2f;
                ParticleSystem::Instance().BurstDust(Vector2{ m_position.x, m_position.y + m_size.y * 0.5f }, 4);
            }
        }
    } else {
        // Idle friction
        m_velocity.x *= (1.0f - 8.0f * dt);
    }

    // Apply gravity
    m_velocity.y += cfg.physics.gravity * dt;
    if (m_velocity.y > cfg.physics.maxFallSpeed) {
        m_velocity.y = cfg.physics.maxFallSpeed;
    }

    // Physics
    AABB box = GetBounds();
    CollisionResult col = world.MoveAndSlide(box, m_velocity, dt);
    m_position.x = box.x + box.width * 0.5f;
    m_position.y = box.y + box.height * 0.5f;

    // If chasing and blocked by wall horizontally, jump!
    if (col.collidedX && m_jumpCooldown <= 0.0f && col.grounded) {
        m_velocity.y = -280.0f;
        m_jumpCooldown = 0.8f;
    }

    m_animTimer += dt;
}

void Chaser::Draw() const {
    if (!m_isAlive) return;

    Texture2D tex = AssetManager::Instance().GetTexture(TextureId::Chaser);
    float dirX = (m_facing == Direction::Left) ? -1.0f : 1.0f;

    float runBob = m_isChasing ? std::sin(m_animTimer * 14.0f) * 2.0f : 0.0f;

    if (tex.id > 0) {
        Rectangle src = { 0, 0, (dirX < 0) ? -32.0f : 32.0f, 32.0f };
        Rectangle dest = { m_position.x, m_position.y + runBob, 30.0f, 30.0f };
        Vector2 origin = { 15.0f, 15.0f };
        Color tint = (m_hurtFlashTimer > 0.0f) ? Color{ 255, 100, 100, 255 } : WHITE;
        DrawTexturePro(tex, src, dest, origin, 0.0f, tint);
    } else {
        Color c = (m_hurtFlashTimer > 0.0f) ? WHITE : Color{ 180, 50, 50, 255 };
        DrawRectangle(static_cast<int>(m_position.x - m_size.x * 0.5f),
                      static_cast<int>(m_position.y - m_size.y * 0.5f + runBob),
                      static_cast<int>(m_size.x),
                      static_cast<int>(m_size.y), c);
    }

    // Alert indicator when chasing
    if (m_isChasing) {
        DrawText("!", static_cast<int>(m_position.x - 2.0f), static_cast<int>(m_position.y - m_size.y * 0.5f - 12.0f), 10, YELLOW);
    }

    // Health Bar
    if (m_health < m_maxHealth) {
        float hpPct = m_health / m_maxHealth;
        DrawRectangle(static_cast<int>(m_position.x - 12.0f), static_cast<int>(m_position.y - m_size.y * 0.5f - 6.0f), 24, 3, BLACK);
        DrawRectangle(static_cast<int>(m_position.x - 12.0f), static_cast<int>(m_position.y - m_size.y * 0.5f - 6.0f), static_cast<int>(24.0f * hpPct), 3, RED);
    }

    if (GetGameConfig().debugHitboxes) {
        DrawRectangleLinesEx(GetBounds().ToRectangle(), 1.0f, RED);
    }
}

} // namespace riftbound

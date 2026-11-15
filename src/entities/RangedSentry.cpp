#include "entities/RangedSentry.hpp"
#include "core/Config.hpp"
#include "core/AssetManager.hpp"
#include "core/AudioPlayer.hpp"
#include "core/ParticleSystem.hpp"
#include <cmath>

namespace riftbound {

RangedSentry::RangedSentry(Vector2 pos, float detectionRange)
    : Enemy(EntityType::RangedSentry, pos, Vector2{ 24.0f, 28.0f }, 35.0f, 15.0f),
      m_detectionRange(detectionRange),
      m_anchorPos(pos) {
    const auto& cfg = GetGameConfig();
    m_health = cfg.enemies.sentryHealth;
    m_maxHealth = cfg.enemies.sentryHealth;
    m_fireInterval = cfg.enemies.sentryFireInterval;
    m_projectileSpeed = cfg.enemies.sentryProjectileSpeed;
    m_fireTimer = m_fireInterval * 0.5f; // Initial offset
}

void RangedSentry::Update(float dt, PhysicsWorld& /*world*/) {
    if (!m_isAlive) return;

    if (m_hurtFlashTimer > 0.0f) m_hurtFlashTimer -= dt;
    if (m_invulnTimer > 0.0f) m_invulnTimer -= dt;

    m_animTimer += dt;
    // Gentle floating bob
    m_position.y = m_anchorPos.y + std::sin(m_animTimer * 3.0f) * 6.0f;

    // Face target
    if (m_targetPos.x < m_position.x) {
        m_facing = Direction::Left;
    } else {
        m_facing = Direction::Right;
    }

    // Distance to target
    float dx = m_targetPos.x - m_position.x;
    float dy = m_targetPos.y - m_position.y;
    float dist = std::sqrt(dx * dx + dy * dy);

    if (dist <= m_detectionRange) {
        m_fireTimer += dt;

        if (m_fireTimer >= (m_fireInterval - m_telegraphDuration) && !m_isTelegraphing) {
            m_isTelegraphing = true;
        }

        // Particle telegraph charge
        if (m_isTelegraphing) {
            ParticleSystem::Instance().Emit(
                m_position,
                Vector2{ -dx * 0.1f, -dy * 0.1f },
                Color{ 80, 220, 255, 220 },
                Color{ 0, 100, 255, 0 },
                3.0f, 0.2f
            );
        }

        if (m_fireTimer >= m_fireInterval) {
            m_fireTimer = 0.0f;
            m_isTelegraphing = false;
            FireProjectile();
        }
    } else {
        m_isTelegraphing = false;
    }
}

void RangedSentry::FireProjectile() {
    float dx = m_targetPos.x - m_position.x;
    float dy = m_targetPos.y - m_position.y;
    float dist = std::sqrt(dx * dx + dy * dy);
    if (dist < 1.0f) dist = 1.0f;

    Vector2 dir = { dx / dist, dy / dist };
    Vector2 vel = { dir.x * m_projectileSpeed, dir.y * m_projectileSpeed };

    if (m_projectileCallback) {
        m_projectileCallback(m_position, vel, 18.0f, 2.5f, 5.0f, false);
    }

    AudioPlayer::Instance().PlaySoundEffect(SoundId::RangedFire);
    ParticleSystem::Instance().BurstImpact(m_position, Color{ 80, 200, 255, 255 }, 6);
}

void RangedSentry::Draw() const {
    if (!m_isAlive) return;

    Texture2D tex = AssetManager::Instance().GetTexture(TextureId::Sentry);
    float dirX = (m_facing == Direction::Left) ? -1.0f : 1.0f;

    if (tex.id > 0) {
        Rectangle src = { 0, 0, (dirX < 0) ? -32.0f : 32.0f, 32.0f };
        Rectangle dest = { m_position.x, m_position.y, 32.0f, 32.0f };
        Vector2 origin = { 16.0f, 16.0f };
        Color tint = (m_hurtFlashTimer > 0.0f) ? Color{ 255, 120, 120, 255 } : WHITE;
        DrawTexturePro(tex, src, dest, origin, 0.0f, tint);
    } else {
        Color c = (m_hurtFlashTimer > 0.0f) ? WHITE : Color{ 90, 120, 200, 255 };
        DrawCircleV(m_position, 12.0f, c);
    }

    // Telegraph charge glow ring
    if (m_isTelegraphing) {
        DrawCircleLines(static_cast<int>(m_position.x), static_cast<int>(m_position.y), 16.0f, Color{ 80, 220, 255, 200 });
    }

    // Health Bar
    if (m_health < m_maxHealth) {
        float hpPct = m_health / m_maxHealth;
        DrawRectangle(static_cast<int>(m_position.x - 12.0f), static_cast<int>(m_position.y - m_size.y * 0.5f - 8.0f), 24, 3, BLACK);
        DrawRectangle(static_cast<int>(m_position.x - 12.0f), static_cast<int>(m_position.y - m_size.y * 0.5f - 8.0f), static_cast<int>(24.0f * hpPct), 3, RED);
    }

    if (GetGameConfig().debugHitboxes) {
        DrawRectangleLinesEx(GetBounds().ToRectangle(), 1.0f, SKYBLUE);
    }
}

} // namespace riftbound

#include "combat/Projectile.hpp"
#include "core/AssetManager.hpp"
#include "core/ParticleSystem.hpp"
#include <cmath>

namespace riftbound {

void Projectile::Spawn(Vector2 pos, Vector2 vel, float damage, float lifetime, float radius, bool fromPlayer) {
    m_position = pos;
    m_velocity = vel;
    m_damage = damage;
    m_lifetime = 0.0f;
    m_maxLifetime = lifetime;
    m_radius = radius;
    m_fromPlayer = fromPlayer;
    m_active = true;
}

void Projectile::Update(float dt) {
    if (!m_active) return;

    m_lifetime += dt;
    if (m_lifetime >= m_maxLifetime) {
        Deactivate();
        return;
    }

    m_position.x += m_velocity.x * dt;
    m_position.y += m_velocity.y * dt;

    // Trail particles
    if (m_fromPlayer) {
        ParticleSystem::Instance().Emit(
            m_position,
            Vector2{ -m_velocity.x * 0.1f, -m_velocity.y * 0.1f },
            Color{ 80, 220, 255, 200 },
            Color{ 20, 100, 255, 0 },
            2.5f, 0.15f
        );
    } else {
        ParticleSystem::Instance().Emit(
            m_position,
            Vector2{ -m_velocity.x * 0.1f, -m_velocity.y * 0.1f },
            Color{ 255, 120, 30, 200 },
            Color{ 200, 20, 0, 0 },
            3.0f, 0.18f
        );
    }
}

void Projectile::Draw() const {
    if (!m_active) return;

    Texture2D tex = AssetManager::Instance().GetTexture(TextureId::Projectiles);
    if (tex.id > 0) {
        Rectangle src = m_fromPlayer ? Rectangle{ 0, 0, 16, 16 } : Rectangle{ 16, 0, 16, 16 };
        Rectangle dest = { m_position.x, m_position.y, m_radius * 2.5f, m_radius * 2.5f };
        Vector2 origin = { dest.width * 0.5f, dest.height * 0.5f };
        DrawTexturePro(tex, src, dest, origin, 0.0f, WHITE);
    } else {
        Color c = m_fromPlayer ? Color{ 80, 220, 255, 255 } : Color{ 255, 100, 30, 255 };
        DrawCircleV(m_position, m_radius, c);
        DrawCircleV(m_position, m_radius * 0.5f, WHITE);
    }
}

void Projectile::Deactivate() {
    if (m_active) {
        Color sparkCol = m_fromPlayer ? Color{ 100, 230, 255, 255 } : Color{ 255, 120, 40, 255 };
        ParticleSystem::Instance().BurstImpact(m_position, sparkCol, 6);
        m_active = false;
    }
}

AABB Projectile::GetBounds() const {
    return AABB{
        m_position.x - m_radius,
        m_position.y - m_radius,
        m_radius * 2.0f,
        m_radius * 2.0f
    };
}

} // namespace riftbound

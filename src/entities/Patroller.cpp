#include "entities/Patroller.hpp"
#include "core/Config.hpp"
#include "core/AssetManager.hpp"
#include "physics/PhysicsWorld.hpp"
#include <cmath>

namespace riftbound {

Patroller::Patroller(Vector2 pos, float patrolLeft, float patrolRight)
    : Enemy(EntityType::Patroller, pos, Vector2{ 22.0f, 26.0f }, 45.0f, 20.0f),
      m_patrolLeft(patrolLeft),
      m_patrolRight(patrolRight) {
    const auto& cfg = GetGameConfig();
    m_speed = cfg.enemies.patrollerSpeed;
    m_health = cfg.enemies.patrollerHealth;
    m_maxHealth = cfg.enemies.patrollerHealth;
    m_contactDamage = cfg.enemies.patrollerDamage;
    m_size = { cfg.enemies.patrollerWidth, cfg.enemies.patrollerHeight };
    m_facing = Direction::Right;
}

void Patroller::Update(float dt, PhysicsWorld& world) {
    if (!m_isAlive) return;

    if (m_hurtFlashTimer > 0.0f) m_hurtFlashTimer -= dt;
    if (m_invulnTimer > 0.0f) m_invulnTimer -= dt;

    const auto& cfg = GetGameConfig();

    // Horizontal patrol movement
    float dirX = (m_facing == Direction::Left) ? -1.0f : 1.0f;
    m_velocity.x = dirX * m_speed;

    // Check bounds
    if (m_facing == Direction::Right && m_position.x >= m_patrolRight) {
        m_facing = Direction::Left;
    } else if (m_facing == Direction::Left && m_position.x <= m_patrolLeft) {
        m_facing = Direction::Right;
    }

    // Gravity
    m_velocity.y += cfg.physics.gravity * dt;
    if (m_velocity.y > cfg.physics.maxFallSpeed) {
        m_velocity.y = cfg.physics.maxFallSpeed;
    }

    // Move & collide
    AABB box = GetBounds();
    CollisionResult col = world.MoveAndSlide(box, m_velocity, dt);
    m_position.x = box.x + box.width * 0.5f;
    m_position.y = box.y + box.height * 0.5f;

    // Turn around if wall hit
    if (col.collidedX) {
        m_facing = (m_facing == Direction::Left) ? Direction::Right : Direction::Left;
    }

    // Turn around if reaching cliff edge
    Vector2 footProbe = { m_position.x + dirX * 12.0f, m_position.y + m_size.y * 0.5f + 4.0f };
    AABB probeBox = { footProbe.x - 2.0f, footProbe.y, 4.0f, 4.0f };
    if (col.grounded && !world.CheckGrounded(probeBox, 4.0f)) {
        m_facing = (m_facing == Direction::Left) ? Direction::Right : Direction::Left;
    }

    m_animTimer += dt;
}

void Patroller::Draw() const {
    if (!m_isAlive) return;

    Texture2D tex = AssetManager::Instance().GetTexture(TextureId::Patroller);
    float dirX = (m_facing == Direction::Left) ? -1.0f : 1.0f;

    float bobY = std::sin(m_animTimer * 10.0f) * 1.5f;

    if (tex.id > 0) {
        Rectangle src = { 0, 0, (dirX < 0) ? -32.0f : 32.0f, 32.0f };
        Rectangle dest = { m_position.x, m_position.y + bobY, 30.0f, 30.0f };
        Vector2 origin = { 15.0f, 15.0f };
        Color tint = (m_hurtFlashTimer > 0.0f) ? Color{ 255, 100, 100, 255 } : WHITE;
        DrawTexturePro(tex, src, dest, origin, 0.0f, tint);
    } else {
        Color c = (m_hurtFlashTimer > 0.0f) ? WHITE : Color{ 100, 140, 100, 255 };
        DrawRectangle(static_cast<int>(m_position.x - m_size.x * 0.5f),
                      static_cast<int>(m_position.y - m_size.y * 0.5f + bobY),
                      static_cast<int>(m_size.x),
                      static_cast<int>(m_size.y), c);
    }

    // Small HP Bar above head
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

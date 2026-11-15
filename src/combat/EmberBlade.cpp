#include "combat/EmberBlade.hpp"
#include "core/Config.hpp"
#include "core/AudioPlayer.hpp"
#include "core/ParticleSystem.hpp"
#include "core/AssetManager.hpp"
#include <cmath>

namespace riftbound {

EmberBlade::EmberBlade() {
    const auto& cfg = GetGameConfig();
    m_damage = cfg.weapons.bladeDamage;
    m_cooldownDuration = cfg.weapons.bladeCooldown;
    m_reachX = cfg.weapons.bladeReachX;
    m_reachY = cfg.weapons.bladeReachY;
    m_knockback = cfg.weapons.bladeKnockback;
}

void EmberBlade::Update(float dt) {
    if (m_cooldownTimer > 0.0f) {
        m_cooldownTimer -= dt;
    }

    if (m_slashActive) {
        m_slashTimer += dt;
        if (m_slashTimer >= m_slashDuration) {
            m_slashActive = false;
        }
    }
}

bool EmberBlade::Attack(Vector2 userPos, Direction facingDir) {
    if (!CanAttack()) return false;

    m_cooldownTimer = m_cooldownDuration;
    m_slashActive = true;
    m_slashTimer = 0.0f;
    m_slashDir = (facingDir == Direction::Left) ? Direction::Left : Direction::Right;
    m_slashPos = userPos;
    m_attackId++;
    m_hitEntities.clear();

    AudioPlayer::Instance().PlaySoundEffect(SoundId::MeleeSlash);

    // Emit ember slash sparks
    float dirX = (m_slashDir == Direction::Left) ? -1.0f : 1.0f;
    Vector2 sparkOrigin = { userPos.x + dirX * 18.0f, userPos.y };
    ParticleSystem::Instance().BurstEmberSparks(sparkOrigin, Vector2{ dirX, 0.0f }, 8);

    return true;
}

AABB EmberBlade::GetHitbox(Vector2 userPos, Direction facingDir) const {
    float dirX = (facingDir == Direction::Left) ? -1.0f : 1.0f;
    float startX = (dirX > 0.0f) ? userPos.x : (userPos.x - m_reachX);
    float startY = userPos.y - m_reachY * 0.5f;

    return AABB{ startX, startY, m_reachX, m_reachY };
}

bool EmberBlade::HasHitEntity(uint32_t entityId) const {
    return m_hitEntities.find(entityId) != m_hitEntities.end();
}

void EmberBlade::RegisterHitEntity(uint32_t entityId) {
    m_hitEntities.insert(entityId);
}

void EmberBlade::Draw(Vector2 userPos, Direction facingDir) const {
    if (!m_slashActive) return;

    Texture2D tex = AssetManager::Instance().GetTexture(TextureId::SlashEffect);
    float dirX = (facingDir == Direction::Left) ? -1.0f : 1.0f;
    Vector2 center = { userPos.x + dirX * 20.0f, userPos.y };

    float progress = m_slashTimer / m_slashDuration;
    float alpha = 1.0f - progress;
    Color tint = Color{ 255, 170, 40, static_cast<unsigned char>(alpha * 255) };

    if (tex.id > 0) {
        Rectangle src = { 0, 0, (dirX < 0) ? -32.0f : 32.0f, 32.0f };
        Rectangle dest = { center.x, center.y, 36.0f, 36.0f };
        Vector2 origin = { dest.width * 0.5f, dest.height * 0.5f };
        DrawTexturePro(tex, src, dest, origin, 0.0f, tint);
    } else {
        // Fallback drawing if texture not present
        DrawCircleSector(center, 24.0f, (dirX > 0) ? -45.0f : 135.0f, (dirX > 0) ? 45.0f : 225.0f, 8, tint);
    }

    if (GetGameConfig().debugHitboxes) {
        AABB box = GetHitbox(userPos, facingDir);
        DrawRectangleLinesEx(box.ToRectangle(), 1.0f, YELLOW);
    }
}

} // namespace riftbound

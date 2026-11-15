#include "level/Collectible.hpp"
#include "core/AssetManager.hpp"
#include "core/AudioPlayer.hpp"
#include "core/ParticleSystem.hpp"
#include <cmath>

namespace riftbound {

Collectible::Collectible(Vector2 pos, CollectibleType type)
    : m_position(pos),
      m_basePos(pos),
      m_type(type) {
}

void Collectible::Update(float dt) {
    if (!m_active) return;

    m_animTimer += dt;
    // Floating bobbing motion
    m_position.y = m_basePos.y + std::sin(m_animTimer * 4.0f) * 4.0f;

    // Sparkle particles occasionally
    if (std::sin(m_animTimer * 8.0f) > 0.85f) {
        Color c = GOLD;
        if (m_type == CollectibleType::Health) c = RED;
        else if (m_type == CollectibleType::Energy) c = SKYBLUE;
        ParticleSystem::Instance().Emit(
            m_position,
            Vector2{ 0.0f, -15.0f },
            c,
            BLANK,
            2.0f, 0.25f
        );
    }
}

bool Collectible::CheckPickup(const AABB& playerBox) {
    if (!m_active) return false;

    if (Intersects(playerBox, GetBounds())) {
        m_active = false;
        Color col = GOLD;
        if (m_type == CollectibleType::Health) {
            col = Color{ 240, 50, 60, 255 };
            AudioPlayer::Instance().PlaySoundEffect(SoundId::PickupHealth);
        } else if (m_type == CollectibleType::Energy) {
            col = Color{ 50, 200, 255, 255 };
            AudioPlayer::Instance().PlaySoundEffect(SoundId::PickupEnergy);
        } else {
            col = Color{ 255, 210, 40, 255 };
            AudioPlayer::Instance().PlaySoundEffect(SoundId::PickupCore);
        }
        ParticleSystem::Instance().BurstCollectible(m_position, col, 14);
        return true;
    }
    return false;
}

AABB Collectible::GetBounds() const {
    return AABB{
        m_position.x - m_radius,
        m_position.y - m_radius,
        m_radius * 2.0f,
        m_radius * 2.0f
    };
}

void Collectible::Draw() const {
    if (!m_active) return;

    Texture2D tex = AssetManager::Instance().GetTexture(TextureId::Collectibles);
    if (tex.id > 0) {
        float srcX = 0.0f;
        if (m_type == CollectibleType::Energy) srcX = 16.0f;
        else if (m_type == CollectibleType::EmberCore) srcX = 32.0f;

        Rectangle src = { srcX, 0.0f, 16.0f, 16.0f };
        Rectangle dest = { m_position.x, m_position.y, 16.0f, 16.0f };
        Vector2 origin = { 8.0f, 8.0f };
        DrawTexturePro(tex, src, dest, origin, 0.0f, WHITE);
    } else {
        Color c = GOLD;
        if (m_type == CollectibleType::Health) c = RED;
        else if (m_type == CollectibleType::Energy) c = SKYBLUE;
        DrawCircleV(m_position, m_radius, c);
        DrawCircleV(m_position, m_radius * 0.5f, WHITE);
    }
}

} // namespace riftbound

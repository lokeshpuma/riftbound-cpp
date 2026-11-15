#include "level/Checkpoint.hpp"
#include "core/AudioPlayer.hpp"
#include "core/ParticleSystem.hpp"
#include <cmath>

namespace riftbound {

Checkpoint::Checkpoint(Vector2 pos)
    : m_position(pos) {
}

void Checkpoint::Update(float dt) {
    m_glowTimer += dt;
    if (m_activated) {
        // Continuous gentle flame beacon particles
        if (std::sin(m_glowTimer * 10.0f) > 0.6f) {
            Vector2 flamePos = { m_position.x, m_position.y - m_height * 0.5f + 4.0f };
            ParticleSystem::Instance().Emit(
                flamePos,
                Vector2{ (((float)rand() / RAND_MAX) - 0.5f) * 10.0f, -25.0f },
                Color{ 60, 240, 220, 255 },
                Color{ 20, 80, 200, 0 },
                3.0f, 0.35f
            );
        }
    }
}

bool Checkpoint::CheckActivate(const AABB& playerBox) {
    if (m_activated) return false;

    if (Intersects(playerBox, GetBounds())) {
        m_activated = true;
        AudioPlayer::Instance().PlaySoundEffect(SoundId::Checkpoint);
        ParticleSystem::Instance().BurstCheckpoint(Vector2{ m_position.x, m_position.y - m_height * 0.5f });
        return true;
    }
    return false;
}

AABB Checkpoint::GetBounds() const {
    return AABB{
        m_position.x - m_width * 0.5f,
        m_position.y - m_height * 0.5f,
        m_width,
        m_height
    };
}

void Checkpoint::Draw() const {
    // Altar pillar base
    Rectangle baseRect = { m_position.x - 10.0f, m_position.y, 20.0f, 20.0f };
    DrawRectangleRec(baseRect, Color{ 70, 75, 85, 255 });
    DrawRectangleLinesEx(baseRect, 1.0f, Color{ 100, 110, 125, 255 });

    // Brazier bowl
    DrawRectangle(static_cast<int>(m_position.x - 12.0f), static_cast<int>(m_position.y - 4.0f), 24, 6, Color{ 50, 55, 65, 255 });

    // Ember Flame / Beacon Crystal
    float flamePulse = std::sin(m_glowTimer * 8.0f) * 2.0f;
    Vector2 crystalPos = { m_position.x, m_position.y - 10.0f };

    if (m_activated) {
        // Glowing cyan eternal flame
        DrawCircleV(crystalPos, 7.0f + flamePulse, Color{ 80, 240, 220, 160 });
        DrawCircleV(crystalPos, 4.0f, WHITE);
        DrawText("ACTIVATED", static_cast<int>(m_position.x - 28.0f), static_cast<int>(m_position.y - m_height - 4.0f), 8, SKYBLUE);
    } else {
        // Dormant amber ember
        DrawCircleV(crystalPos, 4.0f, Color{ 180, 100, 30, 200 });
        DrawCircleV(crystalPos, 2.0f, Color{ 255, 170, 50, 255 });
        DrawText("CHECKPOINT", static_cast<int>(m_position.x - 30.0f), static_cast<int>(m_position.y - m_height - 4.0f), 8, LIGHTGRAY);
    }
}

} // namespace riftbound

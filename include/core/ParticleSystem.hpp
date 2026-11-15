#pragma once

#include <raylib.h>
#include <vector>

namespace riftbound {

struct Particle {
    Vector2 position = { 0.0f, 0.0f };
    Vector2 velocity = { 0.0f, 0.0f };
    Color startColor = WHITE;
    Color endColor = BLANK;
    float size = 4.0f;
    float currentLife = 0.0f;
    float maxLife = 0.5f;
    bool active = false;
};

class ParticleSystem {
public:
    static ParticleSystem& Instance();

    void Init(size_t maxParticles = 600);
    void Update(float dt);
    void Draw() const;
    void Clear();

    // Spawning effects
    void Emit(Vector2 pos, Vector2 vel, Color startCol, Color endCol, float size, float life);
    void BurstDust(Vector2 pos, int count = 6);
    void BurstEmberSparks(Vector2 pos, Vector2 dir, int count = 10);
    void BurstImpact(Vector2 pos, Color col = ORANGE, int count = 8);
    void BurstCollectible(Vector2 pos, Color col = GOLD, int count = 14);
    void BurstCheckpoint(Vector2 pos);
    void BurstBossSlam(Vector2 pos, float radius = 60.0f);

    // Screen Shake
    void TriggerScreenShake(float intensity, float duration);
    Vector2 GetScreenShakeOffset() const { return m_shakeOffset; }

private:
    ParticleSystem() = default;

    std::vector<Particle> m_pool;
    size_t m_nextAvailableIndex = 0;

    // Screen shake state
    float m_shakeIntensity = 0.0f;
    float m_shakeTimer = 0.0f;
    Vector2 m_shakeOffset = { 0.0f, 0.0f };
};

} // namespace riftbound

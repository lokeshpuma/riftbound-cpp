#include "core/ParticleSystem.hpp"
#include "core/Config.hpp"
#include <cmath>
#include <cstdlib>

namespace riftbound {

static inline float RandFloat() {
    return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
}

ParticleSystem& ParticleSystem::Instance() {
    static ParticleSystem instance;
    return instance;
}

void ParticleSystem::Init(size_t maxParticles) {
    m_pool.resize(maxParticles);
    m_nextAvailableIndex = 0;
    m_shakeIntensity = 0.0f;
    m_shakeTimer = 0.0f;
    m_shakeOffset = { 0.0f, 0.0f };
}

void ParticleSystem::Clear() {
    for (auto& p : m_pool) {
        p.active = false;
    }
    m_shakeIntensity = 0.0f;
    m_shakeTimer = 0.0f;
    m_shakeOffset = { 0.0f, 0.0f };
}

void ParticleSystem::Emit(Vector2 pos, Vector2 vel, Color startCol, Color endCol, float size, float life) {
    if (m_pool.empty()) return;

    for (size_t i = 0; i < m_pool.size(); ++i) {
        size_t idx = (m_nextAvailableIndex + i) % m_pool.size();
        if (!m_pool[idx].active) {
            m_pool[idx].position = pos;
            m_pool[idx].velocity = vel;
            m_pool[idx].startColor = startCol;
            m_pool[idx].endColor = endCol;
            m_pool[idx].size = size;
            m_pool[idx].currentLife = 0.0f;
            m_pool[idx].maxLife = life;
            m_pool[idx].active = true;
            m_nextAvailableIndex = (idx + 1) % m_pool.size();
            return;
        }
    }
}

void ParticleSystem::Update(float dt) {
    if (dt <= 0.0f) return;

    // Update particles
    for (auto& p : m_pool) {
        if (!p.active) continue;

        p.currentLife += dt;
        if (p.currentLife >= p.maxLife) {
            p.active = false;
            continue;
        }

        p.position.x += p.velocity.x * dt;
        p.position.y += p.velocity.y * dt;
        // Minor drag
        p.velocity.x *= (1.0f - 2.0f * dt);
        p.velocity.y += 120.0f * dt; // slight gravity
    }

    // Update screen shake
    if (m_shakeTimer > 0.0f) {
        m_shakeTimer -= dt;
        if (m_shakeTimer <= 0.0f) {
            m_shakeOffset = { 0.0f, 0.0f };
            m_shakeIntensity = 0.0f;
        } else {
            if (GetGameConfig().screenShakeEnabled) {
                float offsetX = (RandFloat() * 2.0f - 1.0f) * m_shakeIntensity;
                float offsetY = (RandFloat() * 2.0f - 1.0f) * m_shakeIntensity;
                m_shakeOffset = { offsetX, offsetY };
            } else {
                m_shakeOffset = { 0.0f, 0.0f };
            }
        }
    }
}

void ParticleSystem::Draw() const {
    for (const auto& p : m_pool) {
        if (!p.active) continue;

        float t = p.currentLife / p.maxLife;
        Color c;
        c.r = static_cast<unsigned char>(p.startColor.r + (p.endColor.r - p.startColor.r) * t);
        c.g = static_cast<unsigned char>(p.startColor.g + (p.endColor.g - p.startColor.g) * t);
        c.b = static_cast<unsigned char>(p.startColor.b + (p.endColor.b - p.startColor.b) * t);
        c.a = static_cast<unsigned char>(p.startColor.a + (p.endColor.a - p.startColor.a) * t);

        float curSize = p.size * (1.0f - t * 0.5f);
        DrawRectangle(static_cast<int>(p.position.x - curSize * 0.5f),
                      static_cast<int>(p.position.y - curSize * 0.5f),
                      static_cast<int>(curSize),
                      static_cast<int>(curSize), c);
    }
}

void ParticleSystem::BurstDust(Vector2 pos, int count) {
    for (int i = 0; i < count; ++i) {
        float angle = RandFloat() * 3.14159f; // upward half circle
        float speed = 20.0f + RandFloat() * 40.0f;
        Vector2 vel = { std::cos(angle) * speed, -std::abs(std::sin(angle)) * speed * 0.4f };
        Emit(pos, vel, Color{ 180, 180, 190, 180 }, Color{ 100, 100, 110, 0 }, 3.0f, 0.35f);
    }
}

void ParticleSystem::BurstEmberSparks(Vector2 pos, Vector2 dir, int count) {
    for (int i = 0; i < count; ++i) {
        float angleOffset = (RandFloat() - 0.5f) * 1.2f;
        float speed = 120.0f + RandFloat() * 160.0f;
        float baseAngle = std::atan2(dir.y, dir.x) + angleOffset;
        Vector2 vel = { std::cos(baseAngle) * speed, std::sin(baseAngle) * speed };
        Emit(pos, vel, Color{ 255, 140, 30, 255 }, Color{ 255, 30, 10, 0 }, 4.0f, 0.28f);
    }
}

void ParticleSystem::BurstImpact(Vector2 pos, Color col, int count) {
    for (int i = 0; i < count; ++i) {
        float angle = RandFloat() * 6.28318f;
        float speed = 60.0f + RandFloat() * 120.0f;
        Vector2 vel = { std::cos(angle) * speed, std::sin(angle) * speed };
        Emit(pos, vel, col, Color{ col.r, col.g, col.b, 0 }, 3.5f, 0.25f);
    }
}

void ParticleSystem::BurstCollectible(Vector2 pos, Color col, int count) {
    for (int i = 0; i < count; ++i) {
        float angle = RandFloat() * 6.28318f;
        float speed = 40.0f + RandFloat() * 90.0f;
        Vector2 vel = { std::cos(angle) * speed, std::sin(angle) * speed };
        Emit(pos, vel, col, Color{ 255, 255, 255, 0 }, 3.0f, 0.45f);
    }
}

void ParticleSystem::BurstCheckpoint(Vector2 pos) {
    for (int i = 0; i < 30; ++i) {
        float angle = RandFloat() * 6.28318f;
        float speed = 50.0f + RandFloat() * 140.0f;
        Vector2 vel = { std::cos(angle) * speed, std::sin(angle) * speed - 60.0f };
        Emit(pos, vel, Color{ 60, 240, 220, 255 }, Color{ 20, 100, 200, 0 }, 4.0f, 0.6f);
    }
}

void ParticleSystem::BurstBossSlam(Vector2 pos, float radius) {
    for (int i = 0; i < 40; ++i) {
        float spreadX = (RandFloat() * 2.0f - 1.0f) * radius;
        Vector2 p = { pos.x + spreadX, pos.y };
        float vy = -(80.0f + RandFloat() * 180.0f);
        float vx = spreadX * 2.5f;
        Emit(p, Vector2{ vx, vy }, Color{ 255, 100, 20, 255 }, Color{ 80, 20, 0, 0 }, 5.0f, 0.5f);
    }
    TriggerScreenShake(6.0f, 0.35f);
}

void ParticleSystem::TriggerScreenShake(float intensity, float duration) {
    if (intensity > m_shakeIntensity) {
        m_shakeIntensity = intensity;
    }
    m_shakeTimer = duration;
}

} // namespace riftbound

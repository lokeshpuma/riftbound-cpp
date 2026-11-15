#include "combat/PulseBolts.hpp"
#include "core/Config.hpp"
#include "core/AudioPlayer.hpp"
#include "core/ParticleSystem.hpp"

namespace riftbound {

PulseBolts::PulseBolts(SpawnProjectileCallback spawnCb)
    : m_spawnCallback(spawnCb) {
    const auto& cfg = GetGameConfig();
    m_damage = cfg.weapons.boltDamage;
    m_speed = cfg.weapons.boltSpeed;
    m_lifetime = cfg.weapons.boltLifetime;
    m_cooldownDuration = cfg.weapons.boltCooldown;
    m_radius = cfg.weapons.boltRadius;
    m_energyCost = cfg.weapons.boltEnergyCost;
}

void PulseBolts::Update(float dt) {
    if (m_cooldownTimer > 0.0f) {
        m_cooldownTimer -= dt;
    }
}

bool PulseBolts::Attack(Vector2 userPos, Direction facingDir) {
    if (!CanAttack()) return false;

    m_cooldownTimer = m_cooldownDuration;
    float dirX = (facingDir == Direction::Left) ? -1.0f : 1.0f;
    Vector2 spawnPos = { userPos.x + dirX * 16.0f, userPos.y - 2.0f };
    Vector2 vel = { dirX * m_speed, 0.0f };

    if (m_spawnCallback) {
        m_spawnCallback(spawnPos, vel, m_damage, m_lifetime, m_radius, true);
    }

    AudioPlayer::Instance().PlaySoundEffect(SoundId::RangedFire);

    // Muzzle flash particles
    ParticleSystem::Instance().BurstImpact(spawnPos, Color{ 80, 220, 255, 255 }, 5);

    return true;
}

void PulseBolts::Draw(Vector2 /*userPos*/, Direction /*facingDir*/) const {
    // Pulse bolt weapon is held internally, projectile draws itself
}

} // namespace riftbound

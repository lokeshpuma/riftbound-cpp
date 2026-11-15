#include "entities/Enemy.hpp"
#include "core/AudioPlayer.hpp"
#include "core/ParticleSystem.hpp"
#include <cstdlib>

namespace riftbound {

Enemy::Enemy(EntityType type, Vector2 pos, Vector2 size, float maxHp, float contactDamage)
    : Entity(type, pos, size, maxHp),
      m_contactDamage(contactDamage) {
    m_invulnDuration = 0.08f;
}

bool Enemy::TakeDamage(const DamageInfo& info) {
    if (!m_isAlive) return false;

    // Check if hit by the same attack swing
    if (info.attackId > 0 && info.attackId == m_lastHitAttackId) {
        return false;
    }
    m_lastHitAttackId = info.attackId;
    m_invulnTimer = 0.0f; // Allow new attack ID to hit cleanly

    bool tookDmg = Entity::TakeDamage(info);
    if (tookDmg) {
        m_hurtFlashTimer = 0.12f;
        AudioPlayer::Instance().PlaySoundEffect(SoundId::EnemyHurt);
        ParticleSystem::Instance().BurstImpact(m_position, Color{ 255, 140, 40, 255 }, 8);

        if (!m_isAlive) {
            ParticleSystem::Instance().BurstImpact(m_position, Color{ 255, 50, 20, 255 }, 16);
            DropLoot();
        }
    }
    return tookDmg;
}

void Enemy::DropLoot() {
    if (!m_lootCallback) return;

    // 40% chance health, 40% chance energy, 20% chance core
    int r = rand() % 100;
    if (r < 40) {
        m_lootCallback(m_position, CollectibleType::Health);
    } else if (r < 80) {
        m_lootCallback(m_position, CollectibleType::Energy);
    } else {
        m_lootCallback(m_position, CollectibleType::EmberCore);
    }
}

} // namespace riftbound

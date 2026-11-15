#pragma once

#include "entities/Entity.hpp"
#include <functional>
#include <cstdint>

namespace riftbound {

class Enemy : public Entity {
public:
    using SpawnLootCallback = std::function<void(Vector2 pos, CollectibleType type)>;
    using SpawnProjectileCallback = std::function<void(Vector2 pos, Vector2 vel, float damage, float lifetime, float radius, bool fromPlayer)>;

    Enemy(EntityType type, Vector2 pos, Vector2 size, float maxHp, float contactDamage);

    void SetLootCallback(SpawnLootCallback cb) { m_lootCallback = cb; }
    void SetProjectileCallback(SpawnProjectileCallback cb) { m_projectileCallback = cb; }

    float GetContactDamage() const { return m_contactDamage; }
    bool TakeDamage(const DamageInfo& info) override;

    // Last attack ID that hit this enemy (to prevent multi-hit within a single sword slash)
    uint32_t GetLastHitAttackId() const { return m_lastHitAttackId; }
    void SetLastHitAttackId(uint32_t atkId) { m_lastHitAttackId = atkId; }

protected:
    float m_contactDamage = 15.0f;
    uint32_t m_lastHitAttackId = 0;
    float m_hurtFlashTimer = 0.0f;
    SpawnLootCallback m_lootCallback;
    SpawnProjectileCallback m_projectileCallback;

    void DropLoot();
};

} // namespace riftbound

#pragma once

#include "combat/Weapon.hpp"
#include <functional>

namespace riftbound {

class PulseBolts : public Weapon {
public:
    using SpawnProjectileCallback = std::function<void(Vector2 pos, Vector2 vel, float damage, float lifetime, float radius, bool fromPlayer)>;

    PulseBolts(SpawnProjectileCallback spawnCb = nullptr);

    void SetSpawnCallback(SpawnProjectileCallback cb) { m_spawnCallback = cb; }

    void Update(float dt) override;
    bool Attack(Vector2 userPos, Direction facingDir) override;
    void Draw(Vector2 userPos, Direction facingDir) const override;
    WeaponType GetType() const override { return WeaponType::PulseBolts; }
    const char* GetName() const override { return "Pulse Bolts"; }

    float GetEnergyCost() const { return m_energyCost; }

private:
    float m_damage = 22.0f;
    float m_speed = 460.0f;
    float m_lifetime = 1.6f;
    float m_radius = 4.5f;
    float m_energyCost = 14.0f;

    SpawnProjectileCallback m_spawnCallback;
};

} // namespace riftbound

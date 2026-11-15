#pragma once

#include "level/Tile.hpp"
#include "level/Hazard.hpp"
#include "level/Collectible.hpp"
#include "level/Checkpoint.hpp"
#include "physics/PhysicsWorld.hpp"
#include "entities/Enemy.hpp"
#include "entities/Boss.hpp"
#include <string>
#include <vector>
#include <memory>

namespace riftbound {

class Level {
public:
    Level(int levelIndex, std::string name, int widthTiles, int heightTiles);

    void InitPhysics(PhysicsWorld& world);
    void Update(float dt, Vector2 playerPos);
    void DrawBackground(Vector2 cameraPos) const;
    void DrawTiles() const;
    void DrawEntities() const;

    int GetIndex() const { return m_levelIndex; }
    const std::string& GetName() const { return m_name; }
    Vector2 GetSpawnPoint() const { return m_spawnPoint; }
    AABB GetExitTrigger() const { return m_exitTrigger; }
    float GetPixelWidth() const { return static_cast<float>(m_widthTiles * TILE_SIZE); }
    float GetPixelHeight() const { return static_cast<float>(m_heightTiles * TILE_SIZE); }

    std::vector<std::unique_ptr<Enemy>>& GetEnemies() { return m_enemies; }
    std::vector<Collectible>& GetCollectibles() { return m_collectibles; }
    std::vector<Checkpoint>& GetCheckpoints() { return m_checkpoints; }
    std::vector<Hazard>& GetHazards() { return m_hazards; }
    AshenWarden* GetBoss() { return m_boss.get(); }

    void SetSpawnPoint(Vector2 pt) { m_spawnPoint = pt; }
    void SetExitTrigger(AABB trigger) { m_exitTrigger = trigger; }
    void SetTile(int x, int y, TileType type);
    TileType GetTile(int x, int y) const;

    void AddEnemy(std::unique_ptr<Enemy> enemy);
    void AddCollectible(Vector2 pos, CollectibleType type);
    void AddCheckpoint(Vector2 pos);
    void AddHazard(AABB box, float damage = 25.0f);
    void SetBoss(std::unique_ptr<AshenWarden> boss);

private:
    int m_levelIndex;
    std::string m_name;
    int m_widthTiles;
    int m_heightTiles;
    Vector2 m_spawnPoint = { 80.0f, 150.0f };
    AABB m_exitTrigger = { 0.0f, 0.0f, 32.0f, 64.0f };

    std::vector<TileType> m_tiles;
    std::vector<std::unique_ptr<Enemy>> m_enemies;
    std::vector<Collectible> m_collectibles;
    std::vector<Checkpoint> m_checkpoints;
    std::vector<Hazard> m_hazards;
    std::unique_ptr<AshenWarden> m_boss;
};

} // namespace riftbound

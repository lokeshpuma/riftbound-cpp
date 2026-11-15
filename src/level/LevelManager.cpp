#include "level/LevelManager.hpp"
#include "entities/Patroller.hpp"
#include "entities/RangedSentry.hpp"
#include "entities/Chaser.hpp"
#include "entities/Boss.hpp"
#include <iostream>

namespace riftbound {

LevelManager& LevelManager::Instance() {
    static LevelManager instance;
    return instance;
}

void LevelManager::Init() {
    m_currentLevelIndex = 1;
}

std::string LevelManager::GetLevelName(int levelIndex) const {
    switch (levelIndex) {
        case 1: return "Level 1: Awakening Ruins";
        case 2: return "Level 2: The Sunken Spire";
        case 3: return "Level 3: The Ashen Crucible";
        default: return "Unknown Level";
    }
}

std::unique_ptr<Level> LevelManager::LoadLevel(int levelIndex) {
    m_currentLevelIndex = levelIndex;
    switch (levelIndex) {
        case 1: return BuildLevel1();
        case 2: return BuildLevel2();
        case 3: return BuildLevel3();
        default: return BuildLevel1();
    }
}

std::unique_ptr<Level> LevelManager::BuildLevel1() {
    // 60 x 18 tiles (1440 x 432 px)
    auto lvl = std::make_unique<Level>(1, "Awakening Ruins", 60, 18);
    lvl->SetSpawnPoint(Vector2{ 72.0f, 280.0f });
    lvl->SetExitTrigger(AABB{ 1340.0f, 260.0f, 32.0f, 60.0f });

    // Main floor across level
    for (int x = 0; x < 60; ++x) {
        // Gap at x=20..22 for pit jump tutorial
        if (x >= 20 && x <= 22) continue;
        lvl->SetTile(x, 14, TileType::SolidGround);
        lvl->SetTile(x, 15, TileType::SolidStone);
        lvl->SetTile(x, 16, TileType::SolidStone);
        lvl->SetTile(x, 17, TileType::SolidStone);
    }

    // Spike hazard in the gap pit
    for (int x = 20; x <= 22; ++x) {
        lvl->SetTile(x, 17, TileType::HazardSpikes);
    }

    // Step-up platforms
    for (int x = 8; x <= 12; ++x) lvl->SetTile(x, 11, TileType::SolidBrick);
    for (int x = 14; x <= 18; ++x) lvl->SetTile(x, 9, TileType::SolidBrick);

    // One-way wooden ledges
    for (int x = 25; x <= 28; ++x) lvl->SetTile(x, 11, TileType::OneWayPlatform);
    for (int x = 32; x <= 36; ++x) lvl->SetTile(x, 9, TileType::OneWayPlatform);
    for (int x = 40; x <= 45; ++x) lvl->SetTile(x, 11, TileType::SolidBrick);

    // Elevated ruin wall
    for (int y = 9; y <= 13; ++y) lvl->SetTile(48, y, TileType::SolidStone);
    for (int x = 49; x <= 58; ++x) lvl->SetTile(x, 9, TileType::SolidBrick);

    // Collectibles
    lvl->AddCollectible(Vector2{ 240.0f, 230.0f }, CollectibleType::Health);
    lvl->AddCollectible(Vector2{ 380.0f, 180.0f }, CollectibleType::Energy);
    lvl->AddCollectible(Vector2{ 720.0f, 230.0f }, CollectibleType::EmberCore);
    lvl->AddCollectible(Vector2{ 1020.0f, 230.0f }, CollectibleType::Health);
    lvl->AddCollectible(Vector2{ 1280.0f, 180.0f }, CollectibleType::EmberCore);

    // Checkpoint
    lvl->AddCheckpoint(Vector2{ 680.0f, 310.0f });

    // Enemies: Patrollers
    lvl->AddEnemy(std::make_unique<Patroller>(Vector2{ 300.0f, 310.0f }, 220.0f, 440.0f));
    lvl->AddEnemy(std::make_unique<Patroller>(Vector2{ 860.0f, 310.0f }, 720.0f, 1060.0f));
    lvl->AddEnemy(std::make_unique<Patroller>(Vector2{ 1260.0f, 190.0f }, 1180.0f, 1360.0f));

    return lvl;
}

std::unique_ptr<Level> LevelManager::BuildLevel2() {
    // 75 x 24 tiles (1800 x 576 px) - High verticality and hazard traversal
    auto lvl = std::make_unique<Level>(2, "The Sunken Spire", 75, 24);
    lvl->SetSpawnPoint(Vector2{ 80.0f, 420.0f });
    lvl->SetExitTrigger(AABB{ 1680.0f, 160.0f, 32.0f, 60.0f });

    // Base floor with hazardous sunken pits
    for (int x = 0; x < 75; ++x) {
        if ((x >= 12 && x <= 16) || (x >= 32 && x <= 38) || (x >= 52 && x <= 58)) {
            // Hazard spike pit
            lvl->SetTile(x, 23, TileType::HazardSpikes);
            continue;
        }
        lvl->SetTile(x, 21, TileType::SolidStone);
        lvl->SetTile(x, 22, TileType::SolidStone);
        lvl->SetTile(x, 23, TileType::SolidStone);
    }

    // Register primary hazard pit areas
    lvl->AddHazard(AABB{ 12.0f * TILE_SIZE, 23.0f * TILE_SIZE, 5.0f * TILE_SIZE, static_cast<float>(TILE_SIZE) }, 30.0f);
    lvl->AddHazard(AABB{ 32.0f * TILE_SIZE, 23.0f * TILE_SIZE, 7.0f * TILE_SIZE, static_cast<float>(TILE_SIZE) }, 30.0f);
    lvl->AddHazard(AABB{ 52.0f * TILE_SIZE, 23.0f * TILE_SIZE, 7.0f * TILE_SIZE, static_cast<float>(TILE_SIZE) }, 30.0f);

    // Mid-tier platforms & vertical ascending route
    for (int x = 6; x <= 11; ++x) lvl->SetTile(x, 18, TileType::SolidBrick);
    for (int x = 17; x <= 22; ++x) lvl->SetTile(x, 15, TileType::OneWayPlatform);
    for (int x = 24; x <= 30; ++x) lvl->SetTile(x, 13, TileType::SolidBrick);

    // Checkpoint 1 on elevated spire
    lvl->AddCheckpoint(Vector2{ 650.0f, 280.0f });

    // Spire ascent
    for (int x = 32; x <= 36; ++x) lvl->SetTile(x, 10, TileType::OneWayPlatform);
    for (int x = 40; x <= 46; ++x) lvl->SetTile(x, 8, TileType::SolidBrick);
    for (int x = 50; x <= 56; ++x) lvl->SetTile(x, 11, TileType::OneWayPlatform);

    // High bridge leading to exit
    for (int x = 60; x <= 74; ++x) {
        lvl->SetTile(x, 10, TileType::SolidBrick);
        lvl->SetTile(x, 11, TileType::SolidStone);
    }

    // Collectibles
    lvl->AddCollectible(Vector2{ 200.0f, 380.0f }, CollectibleType::Health);
    lvl->AddCollectible(Vector2{ 480.0f, 330.0f }, CollectibleType::Energy);
    lvl->AddCollectible(Vector2{ 650.0f, 210.0f }, CollectibleType::EmberCore);
    lvl->AddCollectible(Vector2{ 1040.0f, 160.0f }, CollectibleType::Health);
    lvl->AddCollectible(Vector2{ 1280.0f, 230.0f }, CollectibleType::Energy);
    lvl->AddCollectible(Vector2{ 1500.0f, 210.0f }, CollectibleType::EmberCore);

    // Checkpoint 2
    lvl->AddCheckpoint(Vector2{ 1100.0f, 470.0f });

    // Enemies: Patrollers, Sentries, and Chasers
    lvl->AddEnemy(std::make_unique<Patroller>(Vector2{ 260.0f, 470.0f }, 180.0f, 280.0f));
    lvl->AddEnemy(std::make_unique<RangedSentry>(Vector2{ 500.0f, 280.0f }, 300.0f));
    lvl->AddEnemy(std::make_unique<Chaser>(Vector2{ 850.0f, 470.0f }, 220.0f));
    lvl->AddEnemy(std::make_unique<RangedSentry>(Vector2{ 1050.0f, 150.0f }, 320.0f));
    lvl->AddEnemy(std::make_unique<Chaser>(Vector2{ 1350.0f, 470.0f }, 220.0f));
    lvl->AddEnemy(std::make_unique<Patroller>(Vector2{ 1550.0f, 210.0f }, 1460.0f, 1650.0f));

    return lvl;
}

std::unique_ptr<Level> BuildLevel3() {
    // 80 x 20 tiles (1920 x 480 px) - The Boss Level!
    auto lvl = std::make_unique<Level>(3, "The Ashen Crucible", 80, 20);
    lvl->SetSpawnPoint(Vector2{ 80.0f, 350.0f });
    lvl->SetExitTrigger(AABB{ 1820.0f, 330.0f, 40.0f, 60.0f });

    // Gauntlet platforms leading to the arena (x = 0 to 40)
    for (int x = 0; x <= 14; ++x) {
        lvl->SetTile(x, 16, TileType::SolidBrick);
        lvl->SetTile(x, 17, TileType::SolidStone);
    }

    // Hazard lava pit
    for (int x = 15; x <= 22; ++x) {
        lvl->SetTile(x, 19, TileType::HazardLava);
    }
    // Stepping stones over lava
    for (int x = 16; x <= 18; ++x) lvl->SetTile(x, 14, TileType::OneWayPlatform);
    for (int x = 20; x <= 22; ++x) lvl->SetTile(x, 12, TileType::OneWayPlatform);

    // Pre-boss landing
    for (int x = 24; x <= 38; ++x) {
        lvl->SetTile(x, 16, TileType::SolidBrick);
        lvl->SetTile(x, 17, TileType::SolidStone);
    }

    // Checkpoint right before Boss Arena!
    lvl->AddCheckpoint(Vector2{ 840.0f, 355.0f });

    // Gate pillar before arena
    for (int y = 6; y <= 12; ++y) {
        lvl->SetTile(39, y, TileType::SolidStone);
    }

    // GRAND BOSS ARENA (x = 42 to 76)
    // Solid arena floor
    for (int x = 40; x <= 79; ++x) {
        lvl->SetTile(x, 16, TileType::SolidBrick);
        lvl->SetTile(x, 17, TileType::SolidStone);
        lvl->SetTile(x, 18, TileType::SolidStone);
        lvl->SetTile(x, 19, TileType::SolidStone);
    }

    // Elevated tactical one-way platforms inside arena
    for (int x = 46; x <= 50; ++x) lvl->SetTile(x, 12, TileType::OneWayPlatform);
    for (int x = 66; x <= 70; ++x) lvl->SetTile(x, 12, TileType::OneWayPlatform);
    for (int x = 55; x <= 61; ++x) lvl->SetTile(x, 9, TileType::OneWayPlatform);

    // Collectibles
    lvl->AddCollectible(Vector2{ 240.0f, 350.0f }, CollectibleType::Health);
    lvl->AddCollectible(Vector2{ 420.0f, 260.0f }, CollectibleType::Energy);
    lvl->AddCollectible(Vector2{ 720.0f, 350.0f }, CollectibleType::EmberCore);
    // Arena side pickups
    lvl->AddCollectible(Vector2{ 1150.0f, 260.0f }, CollectibleType::Health);
    lvl->AddCollectible(Vector2{ 1630.0f, 260.0f }, CollectibleType::Energy);

    // Pre-boss guards
    lvl->AddEnemy(std::make_unique<Chaser>(Vector2{ 300.0f, 350.0f }, 220.0f));
    lvl->AddEnemy(std::make_unique<RangedSentry>(Vector2{ 700.0f, 280.0f }, 300.0f));

    // THE ASHEN WARDEN BOSS
    // Arena bounds: left = 41 * 24 = 984, right = 78 * 24 = 1872
    Vector2 bossCenter = { 1450.0f, 340.0f };
    auto boss = std::make_unique<AshenWarden>(bossCenter, 1000.0f, 1820.0f);
    lvl->SetBoss(std::move(boss));

    return lvl;
}

std::unique_ptr<Level> LevelManager::BuildLevel3() {
    return riftbound::BuildLevel3();
}

} // namespace riftbound

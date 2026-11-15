#include "level/Level.hpp"
#include "core/AssetManager.hpp"
#include <cmath>
#include <memory>
#include <string>

namespace riftbound {

Level::Level(int levelIndex, std::string name, int widthTiles, int heightTiles)
    : m_levelIndex(levelIndex),
      m_name(std::move(name)),
      m_widthTiles(widthTiles),
      m_heightTiles(heightTiles),
      m_tiles(widthTiles * heightTiles, TileType::Air) {
}

void Level::SetTile(int x, int y, TileType type) {
    if (x >= 0 && x < m_widthTiles && y >= 0 && y < m_heightTiles) {
        m_tiles[y * m_widthTiles + x] = type;
    }
}

TileType Level::GetTile(int x, int y) const {
    if (x >= 0 && x < m_widthTiles && y >= 0 && y < m_heightTiles) {
        return m_tiles[y * m_widthTiles + x];
    }
    return TileType::Air;
}

void Level::AddEnemy(std::unique_ptr<Enemy> enemy) {
    m_enemies.push_back(std::move(enemy));
}

void Level::AddCollectible(Vector2 pos, CollectibleType type) {
    m_collectibles.emplace_back(pos, type);
}

void Level::AddCheckpoint(Vector2 pos) {
    m_checkpoints.emplace_back(pos);
}

void Level::AddHazard(AABB box, float damage) {
    m_hazards.emplace_back(box, damage);
}

void Level::SetBoss(std::unique_ptr<AshenWarden> boss) {
    m_boss = std::move(boss);
}

void Level::InitPhysics(PhysicsWorld& world) {
    world.Clear();

    // Scan tilegrid and add colliders (combining horizontal solid tiles into single colliders)
    for (int y = 0; y < m_heightTiles; ++y) {
        int startX = -1;

        for (int x = 0; x < m_widthTiles; ++x) {
            TileType t = GetTile(x, y);
            bool isSolid = (t == TileType::SolidGround || t == TileType::SolidStone || t == TileType::SolidBrick);

            if (isSolid) {
                if (startX == -1) {
                    startX = x;
                }
            } else {
                if (startX != -1) {
                    float px = static_cast<float>(startX * TILE_SIZE);
                    float py = static_cast<float>(y * TILE_SIZE);
                    float pw = static_cast<float>((x - startX) * TILE_SIZE);
                    float ph = static_cast<float>(TILE_SIZE);
                    world.AddSolid(AABB{ px, py, pw, ph });
                    startX = -1;
                }

                if (t == TileType::OneWayPlatform) {
                    float px = static_cast<float>(x * TILE_SIZE);
                    float py = static_cast<float>(y * TILE_SIZE);
                    world.AddOneWayPlatform(AABB{ px, py, static_cast<float>(TILE_SIZE), 8.0f });
                } else if (t == TileType::HazardSpikes || t == TileType::HazardLava) {
                    float px = static_cast<float>(x * TILE_SIZE);
                    float py = static_cast<float>(y * TILE_SIZE + 6);
                    world.AddHazard(AABB{ px, py, static_cast<float>(TILE_SIZE), static_cast<float>(TILE_SIZE - 6) }, 25.0f);
                    m_hazards.emplace_back(AABB{ px, py, static_cast<float>(TILE_SIZE), static_cast<float>(TILE_SIZE - 6) }, 25.0f);
                }
            }
        }

        if (startX != -1) {
            float px = static_cast<float>(startX * TILE_SIZE);
            float py = static_cast<float>(y * TILE_SIZE);
            float pw = static_cast<float>((m_widthTiles - startX) * TILE_SIZE);
            float ph = static_cast<float>(TILE_SIZE);
            world.AddSolid(AABB{ px, py, pw, ph });
        }
    }

    // Add boundaries (walls and pit ceiling/bottom)
    float mapW = GetPixelWidth();
    float mapH = GetPixelHeight();
    world.AddSolid(AABB{ -32.0f, 0.0f, 32.0f, mapH });             // Left border
    world.AddSolid(AABB{ mapW, 0.0f, 32.0f, mapH });              // Right border
    world.AddSolid(AABB{ 0.0f, -32.0f, mapW, 32.0f });             // Ceiling
    world.AddHazard(AABB{ 0.0f, mapH + 16.0f, mapW, 64.0f }, 999.0f); // Bottom kill-zone pit
}

void Level::Update(float dt, Vector2 playerPos) {
    for (auto& cp : m_checkpoints) {
        cp.Update(dt);
    }

    for (auto& col : m_collectibles) {
        col.Update(dt);
    }

    for (auto& haz : m_hazards) {
        haz.Update(dt);
    }

    if (m_boss) {
        m_boss->SetTargetPosition(playerPos);
    }
}

void Level::DrawBackground(Vector2 cameraPos) const {
    // Parallax atmospheric backdrop
    ClearBackground(Color{ 18, 16, 26, 255 });

    // Distant mountain silhouettes (moves at 0.15x camera speed)
    float para1X = cameraPos.x * 0.15f;
    for (int i = -1; i < 6; ++i) {
        float peakX = i * 260.0f - para1X;
        Vector2 v1 = { peakX, 260.0f };
        Vector2 v2 = { peakX + 160.0f, 110.0f };
        Vector2 v3 = { peakX + 320.0f, 260.0f };
        DrawTriangle(v1, v3, v2, Color{ 30, 26, 42, 255 });
    }

    // Midground cavern arches / pillars (moves at 0.35x camera speed)
    float para2X = cameraPos.x * 0.35f;
    for (int i = -1; i < 8; ++i) {
        float colX = i * 180.0f - para2X;
        DrawRectangle(static_cast<int>(colX), 60, 24, 280, Color{ 40, 36, 56, 255 });
        DrawRectangle(static_cast<int>(colX - 4.0f), 54, 32, 10, Color{ 50, 46, 70, 255 });
    }
}

void Level::DrawTiles() const {
    Texture2D tex = AssetManager::Instance().GetTexture(TextureId::Tiles);

    for (int y = 0; y < m_heightTiles; ++y) {
        for (int x = 0; x < m_widthTiles; ++x) {
            TileType t = GetTile(x, y);
            if (t == TileType::Air) continue;

            float px = static_cast<float>(x * TILE_SIZE);
            float py = static_cast<float>(y * TILE_SIZE);

            if (tex.id > 0) {
                Rectangle src = { 0, 0, 32, 32 };
                if (t == TileType::SolidBrick || t == TileType::SolidStone) {
                    src = { 32, 0, 32, 32 };
                } else if (t == TileType::OneWayPlatform) {
                    src = { 0, 32, 32, 32 };
                } else if (t == TileType::HazardSpikes || t == TileType::HazardLava) {
                    src = { 32, 32, 32, 32 };
                }

                Rectangle dest = { px, py, static_cast<float>(TILE_SIZE), static_cast<float>(TILE_SIZE) };
                DrawTexturePro(tex, src, dest, Vector2{ 0, 0 }, 0.0f, WHITE);
            } else {
                Color c = DARKGRAY;
                if (t == TileType::SolidGround) c = Color{ 80, 130, 70, 255 };
                else if (t == TileType::OneWayPlatform) c = Color{ 140, 90, 50, 255 };
                else if (t == TileType::HazardSpikes) c = RED;
                DrawRectangle(static_cast<int>(px), static_cast<int>(py), TILE_SIZE, TILE_SIZE, c);
            }
        }
    }

    // Draw exit goal gateway
    DrawRectangleRec(m_exitTrigger.ToRectangle(), Color{ 100, 220, 255, 60 });
    DrawRectangleLinesEx(m_exitTrigger.ToRectangle(), 2.0f, Color{ 140, 240, 255, 200 });
    DrawText("EXIT", static_cast<int>(m_exitTrigger.x + 4.0f), static_cast<int>(m_exitTrigger.y - 12.0f), 10, SKYBLUE);
}

void Level::DrawEntities() const {
    for (const auto& cp : m_checkpoints) {
        cp.Draw();
    }

    for (const auto& col : m_collectibles) {
        col.Draw();
    }

    for (const auto& enemy : m_enemies) {
        if (enemy && enemy->IsAlive()) {
            enemy->Draw();
        }
    }

    if (m_boss && m_boss->IsAlive()) {
        m_boss->Draw();
    }
}

} // namespace riftbound

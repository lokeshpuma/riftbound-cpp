#include "core/AssetManager.hpp"
#include <iostream>

namespace riftbound {

AssetManager& AssetManager::Instance() {
    static AssetManager instance;
    return instance;
}

void AssetManager::Init() {
    if (m_initialized) return;

    GenerateProceduralTextures();
    m_initialized = true;
    std::cout << "[AssetManager] Procedural pixel assets generated successfully.\n";
}

void AssetManager::Shutdown() {
    if (!m_initialized) return;

    for (auto& [id, tex] : m_textures) {
        UnloadTexture(tex);
    }
    m_textures.clear();
    m_initialized = false;
}

Texture2D AssetManager::GetTexture(TextureId id) const {
    auto it = m_textures.find(static_cast<int>(id));
    if (it != m_textures.end()) {
        return it->second;
    }
    // Return empty fallback texture
    return Texture2D{};
}

void AssetManager::GenerateProceduralTextures() {
    m_textures[static_cast<int>(TextureId::Player)] = GeneratePlayerTexture();
    m_textures[static_cast<int>(TextureId::Patroller)] = GenerateEnemyTextures(0);
    m_textures[static_cast<int>(TextureId::Sentry)] = GenerateEnemyTextures(1);
    m_textures[static_cast<int>(TextureId::Chaser)] = GenerateEnemyTextures(2);
    m_textures[static_cast<int>(TextureId::Boss)] = GenerateBossTexture();
    m_textures[static_cast<int>(TextureId::Tiles)] = GenerateTileTexture();
    m_textures[static_cast<int>(TextureId::Collectibles)] = GenerateCollectibleTexture();
    m_textures[static_cast<int>(TextureId::Projectiles)] = GenerateProjectileTexture();
    m_textures[static_cast<int>(TextureId::SlashEffect)] = GenerateSlashTexture();
}

Texture2D AssetManager::GeneratePlayerTexture() {
    // 32x32 pixel hero character: Cloak, ember armor, glowing visor/eye
    Image img = GenImageColor(32, 32, BLANK);

    // Body / Cape (Dark Navy & Crimson)
    ImageDrawRectangle(&img, 8, 12, 16, 16, Color{ 35, 40, 60, 255 });
    ImageDrawRectangle(&img, 10, 14, 12, 14, Color{ 180, 40, 40, 255 });

    // Head / Helm
    ImageDrawRectangle(&img, 10, 6, 12, 8, Color{ 50, 55, 75, 255 });
    ImageDrawRectangle(&img, 11, 7, 10, 6, Color{ 70, 75, 100, 255 });
    // Glowing Ember Visor
    ImageDrawRectangle(&img, 16, 8, 5, 2, Color{ 255, 170, 30, 255 });
    ImageDrawRectangle(&img, 17, 9, 3, 1, Color{ 255, 240, 150, 255 });

    // Boots / Legs
    ImageDrawRectangle(&img, 10, 28, 5, 4, Color{ 25, 25, 35, 255 });
    ImageDrawRectangle(&img, 17, 28, 5, 4, Color{ 25, 25, 35, 255 });

    // Shoulder Pauldrons
    ImageDrawRectangle(&img, 6, 12, 4, 6, Color{ 120, 130, 150, 255 });
    ImageDrawRectangle(&img, 22, 12, 4, 6, Color{ 120, 130, 150, 255 });

    Texture2D tex = LoadTextureFromImage(img);
    UnloadImage(img);
    return tex;
}

Texture2D AssetManager::GenerateEnemyTextures(int type) {
    Image img = GenImageColor(32, 32, BLANK);

    if (type == 0) {
        // Patroller: Armored construct with spiked carapace
        ImageDrawRectangle(&img, 6, 8, 20, 18, Color{ 80, 90, 80, 255 });
        ImageDrawRectangle(&img, 8, 10, 16, 14, Color{ 110, 125, 110, 255 });
        // Spikes on back
        ImageDrawRectangle(&img, 8, 4, 3, 4, Color{ 60, 70, 60, 255 });
        ImageDrawRectangle(&img, 14, 3, 4, 5, Color{ 60, 70, 60, 255 });
        ImageDrawRectangle(&img, 21, 4, 3, 4, Color{ 60, 70, 60, 255 });
        // Menacing red eye
        ImageDrawRectangle(&img, 20, 14, 4, 3, Color{ 230, 40, 40, 255 });
        ImageDrawRectangle(&img, 21, 15, 2, 1, Color{ 255, 180, 180, 255 });
        // Heavy crawler treads/feet
        ImageDrawRectangle(&img, 6, 26, 8, 6, Color{ 40, 45, 40, 255 });
        ImageDrawRectangle(&img, 18, 26, 8, 6, Color{ 40, 45, 40, 255 });
    } else if (type == 1) {
        // Ranged Sentry: Floating orb/turret with rotating core
        ImageDrawCircle(&img, 16, 16, 11, Color{ 60, 60, 90, 255 });
        ImageDrawCircle(&img, 16, 16, 8, Color{ 90, 100, 140, 255 });
        // Plasma Eye Core
        ImageDrawCircle(&img, 16, 16, 4, Color{ 80, 220, 255, 255 });
        ImageDrawCircle(&img, 16, 16, 2, WHITE);
        // Outer mechanical stabilizer fins
        ImageDrawRectangle(&img, 2, 14, 4, 4, Color{ 140, 150, 180, 255 });
        ImageDrawRectangle(&img, 26, 14, 4, 4, Color{ 140, 150, 180, 255 });
        ImageDrawRectangle(&img, 14, 2, 4, 4, Color{ 140, 150, 180, 255 });
        ImageDrawRectangle(&img, 14, 26, 4, 4, Color{ 140, 150, 180, 255 });
    } else {
        // Chaser: Swift feline/insectoid predator
        ImageDrawRectangle(&img, 6, 10, 20, 14, Color{ 140, 50, 50, 255 });
        ImageDrawRectangle(&img, 8, 12, 16, 10, Color{ 180, 70, 70, 255 });
        // Aggressive yellow slitted eyes
        ImageDrawRectangle(&img, 20, 12, 4, 2, Color{ 255, 220, 40, 255 });
        // Sharp fangs / claws
        ImageDrawRectangle(&img, 22, 18, 3, 4, WHITE);
        // Spring-loaded hind legs
        ImageDrawRectangle(&img, 6, 22, 5, 8, Color{ 90, 30, 30, 255 });
        ImageDrawRectangle(&img, 19, 22, 5, 8, Color{ 90, 30, 30, 255 });
    }

    Texture2D tex = LoadTextureFromImage(img);
    UnloadImage(img);
    return tex;
}

Texture2D AssetManager::GenerateBossTexture() {
    // 64x64 Ashen Warden: Hulking ancient armored golem forged of brimstone and iron
    Image img = GenImageColor(64, 64, BLANK);

    // Torso / Obsidian Armor
    ImageDrawRectangle(&img, 16, 16, 32, 34, Color{ 40, 35, 45, 255 });
    ImageDrawRectangle(&img, 20, 20, 24, 26, Color{ 65, 55, 75, 255 });

    // Fiery Core in Chest (Pulsing magma fissures)
    ImageDrawRectangle(&img, 26, 26, 12, 14, Color{ 240, 90, 20, 255 });
    ImageDrawRectangle(&img, 28, 28, 8, 10, Color{ 255, 190, 40, 255 });
    ImageDrawRectangle(&img, 30, 30, 4, 6, WHITE);

    // Horned Skull / Iron Visage
    ImageDrawRectangle(&img, 22, 6, 20, 12, Color{ 55, 50, 65, 255 });
    // Curving horns
    ImageDrawRectangle(&img, 16, 2, 8, 6, Color{ 130, 40, 30, 255 });
    ImageDrawRectangle(&img, 40, 2, 8, 6, Color{ 130, 40, 30, 255 });
    // Glowing molten eyes
    ImageDrawRectangle(&img, 26, 11, 4, 2, Color{ 255, 120, 0, 255 });
    ImageDrawRectangle(&img, 34, 11, 4, 2, Color{ 255, 120, 0, 255 });

    // Massive Iron Shoulders
    ImageDrawRectangle(&img, 8, 14, 10, 16, Color{ 75, 70, 90, 255 });
    ImageDrawRectangle(&img, 46, 14, 10, 16, Color{ 75, 70, 90, 255 });

    // Sturdy Stone Legs
    ImageDrawRectangle(&img, 18, 50, 11, 14, Color{ 35, 30, 40, 255 });
    ImageDrawRectangle(&img, 35, 50, 11, 14, Color{ 35, 30, 40, 255 });

    Texture2D tex = LoadTextureFromImage(img);
    UnloadImage(img);
    return tex;
}

Texture2D AssetManager::GenerateTileTexture() {
    // 64x64 tilesheet containing 4 tiles (32x32 each):
    // Top-left: Solid Surface Grass/Stone
    // Top-right: Solid Dungeon Brick
    // Bottom-left: One-way Wooden Platform
    // Bottom-right: Spikes & Lava hazard
    Image img = GenImageColor(64, 64, BLANK);

    // Tile 1 (0,0): Solid Stone / Grass Top
    ImageDrawRectangle(&img, 0, 0, 32, 32, Color{ 70, 75, 85, 255 });
    ImageDrawRectangle(&img, 0, 0, 32, 6, Color{ 90, 145, 70, 255 }); // Grass cap
    ImageDrawRectangle(&img, 0, 6, 32, 2, Color{ 60, 110, 50, 255 });
    ImageDrawRectangle(&img, 4, 12, 10, 6, Color{ 85, 90, 105, 255 });
    ImageDrawRectangle(&img, 18, 20, 10, 6, Color{ 85, 90, 105, 255 });

    // Tile 2 (32,0): Dungeon Brick
    ImageDrawRectangle(&img, 32, 0, 32, 32, Color{ 55, 50, 60, 255 });
    ImageDrawRectangleLines(&img, 32, 0, 32, 16, Color{ 35, 30, 40, 255 });
    ImageDrawRectangleLines(&img, 32, 16, 32, 16, Color{ 35, 30, 40, 255 });
    ImageDrawLine(&img, 48, 0, 48, 16, Color{ 35, 30, 40, 255 });
    ImageDrawLine(&img, 40, 16, 40, 32, Color{ 35, 30, 40, 255 });

    // Tile 3 (0,32): One-Way Wooden Platform
    ImageDrawRectangle(&img, 0, 32, 32, 8, Color{ 140, 95, 55, 255 });
    ImageDrawRectangle(&img, 0, 32, 32, 2, Color{ 180, 130, 85, 255 });
    ImageDrawLine(&img, 10, 34, 10, 40, Color{ 100, 65, 35, 255 });
    ImageDrawLine(&img, 22, 34, 22, 40, Color{ 100, 65, 35, 255 });

    // Tile 4 (32,32): Spikes & Magma
    ImageDrawRectangle(&img, 32, 32, 32, 32, BLANK);
    // Spikes (3 sharp metal triangles)
    for (int i = 0; i < 3; ++i) {
        int sx = 34 + i * 10;
        ImageDrawTriangle(&img, Vector2{ (float)sx, 63 }, Vector2{ (float)(sx + 8), 63 }, Vector2{ (float)(sx + 4), 42 }, Color{ 190, 50, 40, 255 });
        ImageDrawTriangle(&img, Vector2{ (float)(sx + 2), 63 }, Vector2{ (float)(sx + 6), 63 }, Vector2{ (float)(sx + 4), 45 }, Color{ 230, 100, 70, 255 });
    }

    Texture2D tex = LoadTextureFromImage(img);
    UnloadImage(img);
    return tex;
}

Texture2D AssetManager::GenerateCollectibleTexture() {
    // 48x16 sheet with 3 items (16x16 each):
    // 0..15: Health Potion / Heart
    // 16..31: Pulse Energy Crystal
    // 32..47: Ember Core
    Image img = GenImageColor(48, 16, BLANK);

    // 1. Health Heart
    ImageDrawCircle(&img, 5, 6, 4, Color{ 230, 40, 50, 255 });
    ImageDrawCircle(&img, 10, 6, 4, Color{ 230, 40, 50, 255 });
    ImageDrawTriangle(&img, Vector2{ 2, 7 }, Vector2{ 14, 7 }, Vector2{ 8, 14 }, Color{ 230, 40, 50, 255 });
    ImageDrawCircle(&img, 5, 5, 1, WHITE);

    // 2. Pulse Energy Crystal
    ImageDrawRectangle(&img, 22, 3, 5, 10, Color{ 40, 190, 240, 255 });
    ImageDrawTriangle(&img, Vector2{ 22, 3 }, Vector2{ 27, 3 }, Vector2{ 24, 0 }, Color{ 90, 220, 255, 255 });
    ImageDrawTriangle(&img, Vector2{ 22, 13 }, Vector2{ 27, 13 }, Vector2{ 24, 16 }, Color{ 20, 140, 200, 255 });
    ImageDrawRectangle(&img, 23, 5, 2, 6, WHITE);

    // 3. Ember Core (Sun-gold ancient artifact)
    ImageDrawCircle(&img, 40, 8, 6, Color{ 255, 170, 20, 255 });
    ImageDrawCircle(&img, 40, 8, 4, Color{ 255, 225, 60, 255 });
    ImageDrawCircle(&img, 40, 8, 2, WHITE);

    Texture2D tex = LoadTextureFromImage(img);
    UnloadImage(img);
    return tex;
}

Texture2D AssetManager::GenerateProjectileTexture() {
    // 32x16 sheet:
    // Left: Player Pulse Bolt (cyan glow)
    // Right: Sentry / Boss Fire Orb (magma orange glow)
    Image img = GenImageColor(32, 16, BLANK);

    // Player Bolt
    ImageDrawCircle(&img, 8, 8, 6, Color{ 60, 200, 255, 160 });
    ImageDrawCircle(&img, 8, 8, 4, Color{ 120, 230, 255, 255 });
    ImageDrawCircle(&img, 8, 8, 2, WHITE);

    // Enemy / Boss Fire Bolt
    ImageDrawCircle(&img, 24, 8, 7, Color{ 240, 80, 20, 180 });
    ImageDrawCircle(&img, 24, 8, 5, Color{ 255, 160, 40, 255 });
    ImageDrawCircle(&img, 24, 8, 2, WHITE);

    Texture2D tex = LoadTextureFromImage(img);
    UnloadImage(img);
    return tex;
}

Texture2D AssetManager::GenerateSlashTexture() {
    // 32x32 Melee blade slash arc
    Image img = GenImageColor(32, 32, BLANK);

    // Ember arc curve
    for (int deg = -45; deg <= 45; deg += 4) {
        float rad = deg * (3.14159f / 180.0f);
        int cx = static_cast<int>(16 + std::cos(rad) * 12.0f);
        int cy = static_cast<int>(16 + std::sin(rad) * 12.0f);
        ImageDrawCircle(&img, cx, cy, 3, Color{ 255, 140, 30, 200 });
        ImageDrawCircle(&img, cx, cy, 1, WHITE);
    }

    Texture2D tex = LoadTextureFromImage(img);
    UnloadImage(img);
    return tex;
}

} // namespace riftbound

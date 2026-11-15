#pragma once

#include <raylib.h>
#include <string>
#include <unordered_map>

namespace riftbound {

enum class TextureId {
    Player,
    Patroller,
    Sentry,
    Chaser,
    Boss,
    Tiles,
    Collectibles,
    Projectiles,
    SlashEffect
};

class AssetManager {
public:
    static AssetManager& Instance();

    void Init();
    void Shutdown();

    Texture2D GetTexture(TextureId id) const;

private:
    AssetManager() = default;

    std::unordered_map<int, Texture2D> m_textures;
    bool m_initialized = false;

    void GenerateProceduralTextures();
    Texture2D GeneratePlayerTexture();
    Texture2D GenerateEnemyTextures(int type);
    Texture2D GenerateBossTexture();
    Texture2D GenerateTileTexture();
    Texture2D GenerateCollectibleTexture();
    Texture2D GenerateProjectileTexture();
    Texture2D GenerateSlashTexture();
};

} // namespace riftbound

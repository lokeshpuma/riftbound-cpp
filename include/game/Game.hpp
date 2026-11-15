#pragma once

#include "core/Types.hpp"
#include "physics/PhysicsWorld.hpp"
#include "entities/Player.hpp"
#include "combat/Projectile.hpp"
#include "level/Level.hpp"
#include "level/CameraController.hpp"
#include "ui/HUD.hpp"
#include "ui/Menu.hpp"
#include <memory>
#include <vector>
#include <string>

namespace riftbound {

class Game {
public:
    static Game& Instance();

    bool Init();
    void Shutdown();
    void Step(); // Called once per frame (compatible with emscripten_set_main_loop)
    bool ShouldClose() const;

    void ChangeState(GameStateId newState);
    GameStateId GetCurrentState() const { return m_currentState; }

    void LoadLevel(int levelIdx);
    void RestartCurrentLevel();
    void RetryFromCheckpoint();

    void SpawnProjectile(Vector2 pos, Vector2 vel, float damage, float lifetime, float radius, bool fromPlayer);
    void SpawnLoot(Vector2 pos, CollectibleType type);

private:
    Game() = default;

    bool m_isRunning = false;
    GameStateId m_currentState = GameStateId::Boot;
    GameStateId m_previousState = GameStateId::MainMenu;

    // Rendering target for virtual resolution scaling
    RenderTexture2D m_target{};
    int m_virtualWidth = 640;
    int m_virtualHeight = 360;

    // Subsystems
    PhysicsWorld m_physicsWorld;
    std::unique_ptr<Player> m_player;
    std::unique_ptr<Level> m_currentLevel;
    std::unique_ptr<CameraController> m_cameraController;
    HUD m_hud;
    std::unique_ptr<MenuManager> m_menuManager;

    // Object pool for projectiles
    std::vector<Projectile> m_projectiles;

    // Progression
    int m_currentScore = 0;
    int m_collectedCores = 0;
    bool m_bossArenaLocked = false;

    void Update(float dt);
    void Draw();

    void UpdateCombat(float dt);
    void UpdateProjectiles(float dt);
    void CheckTriggers();
    void HandleMenuAction(const std::string& action, int param);

    Vector2 GetVirtualMousePosition() const;
};

} // namespace riftbound

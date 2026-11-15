#include "game/Game.hpp"
#include "core/Config.hpp"
#include "core/AssetManager.hpp"
#include "core/AudioPlayer.hpp"
#include "core/SaveSystem.hpp"
#include "core/ParticleSystem.hpp"
#include "core/InputManager.hpp"
#include "level/LevelManager.hpp"
#include <iostream>
#include <algorithm>

namespace riftbound {

Game& Game::Instance() {
    static Game instance;
    return instance;
}

bool Game::Init() {
    const auto& cfg = GetGameConfig();

    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    InitWindow(cfg.display.windowWidth, cfg.display.windowHeight, cfg.display.windowTitle);
    SetTargetFPS(cfg.display.targetFps);

    m_virtualWidth = cfg.display.virtualWidth;
    m_virtualHeight = cfg.display.virtualHeight;
    m_target = LoadRenderTexture(m_virtualWidth, m_virtualHeight);
    SetTextureFilter(m_target.texture, TEXTURE_FILTER_POINT); // Crisp pixel art

    AssetManager::Instance().Init();
    AudioPlayer::Instance().Init();
    ParticleSystem::Instance().Init(600);
    SaveSystem::Instance().Load();

    m_cameraController = std::make_unique<CameraController>(m_virtualWidth, m_virtualHeight);
    m_menuManager = std::make_unique<MenuManager>([this](const std::string& action, int param) {
        HandleMenuAction(action, param);
    });

    m_projectiles.resize(100); // 100 pooled projectiles

    m_isRunning = true;
    ChangeState(GameStateId::MainMenu);

    std::cout << "[Game] Riftbound engine initialized successfully.\n";
    return true;
}

void Game::Shutdown() {
    if (!m_isRunning) return;

    m_currentLevel.reset();
    m_player.reset();

    UnloadRenderTexture(m_target);
    ParticleSystem::Instance().Clear();
    AssetManager::Instance().Shutdown();
    AudioPlayer::Instance().Shutdown();
    CloseWindow();

    m_isRunning = false;
    std::cout << "[Game] Engine shutdown complete.\n";
}

bool Game::ShouldClose() const {
    return !m_isRunning || WindowShouldClose();
}

void Game::ChangeState(GameStateId newState) {
    m_previousState = m_currentState;
    m_currentState = newState;

    if (newState == GameStateId::Paused) {
        // Freeze gameplay
    } else if (newState == GameStateId::MainMenu) {
        // Clean up active level resources
        m_currentLevel.reset();
        m_player.reset();
    }
}

void Game::LoadLevel(int levelIdx) {
    m_currentLevel = LevelManager::Instance().LoadLevel(levelIdx);
    if (!m_currentLevel) return;

    m_currentLevel->InitPhysics(m_physicsWorld);

    // Initialize player
    Vector2 spawn = m_currentLevel->GetSpawnPoint();
    m_player = std::make_unique<Player>(spawn);
    m_player->GetPulseBolts().SetSpawnCallback([this](Vector2 pos, Vector2 vel, float dmg, float life, float rad, bool fromPlr) {
        SpawnProjectile(pos, vel, dmg, life, rad, fromPlr);
    });

    // Setup callbacks for enemies
    for (auto& enemy : m_currentLevel->GetEnemies()) {
        if (!enemy) continue;
        enemy->SetLootCallback([this](Vector2 pos, CollectibleType type) {
            SpawnLoot(pos, type);
        });
        enemy->SetProjectileCallback([this](Vector2 pos, Vector2 vel, float dmg, float life, float rad, bool fromPlr) {
            SpawnProjectile(pos, vel, dmg, life, rad, fromPlr);
        });
    }

    if (auto* boss = m_currentLevel->GetBoss()) {
        boss->SetLootCallback([this](Vector2 pos, CollectibleType type) {
            SpawnLoot(pos, type);
        });
        boss->SetProjectileCallback([this](Vector2 pos, Vector2 vel, float dmg, float life, float rad, bool fromPlr) {
            SpawnProjectile(pos, vel, dmg, life, rad, fromPlr);
        });
    }

    // Configure camera
    m_cameraController->SetBounds(0.0f, 0.0f, m_currentLevel->GetPixelWidth(), m_currentLevel->GetPixelHeight());
    m_cameraController->SnapTo(spawn);

    // Clear projectiles & particles
    for (auto& p : m_projectiles) {
        p.Deactivate();
    }
    ParticleSystem::Instance().Clear();

    m_bossArenaLocked = false;
    ChangeState(GameStateId::Playing);
}

void Game::RestartCurrentLevel() {
    int cur = LevelManager::Instance().GetCurrentLevelIndex();
    LoadLevel(cur);
}

void Game::RetryFromCheckpoint() {
    if (m_player) {
        m_player->RespawnAtCheckpoint();
        if (m_currentLevel && m_currentLevel->GetBoss()) {
            m_currentLevel->GetBoss()->Reset();
            m_bossArenaLocked = false;
        }
        ChangeState(GameStateId::Playing);
    } else {
        RestartCurrentLevel();
    }
}

void Game::SpawnProjectile(Vector2 pos, Vector2 vel, float damage, float lifetime, float radius, bool fromPlayer) {
    for (auto& p : m_projectiles) {
        if (!p.IsActive()) {
            p.Spawn(pos, vel, damage, lifetime, radius, fromPlayer);
            return;
        }
    }
}

void Game::SpawnLoot(Vector2 pos, CollectibleType type) {
    if (m_currentLevel) {
        m_currentLevel->AddCollectible(pos, type);
    }
}

void Game::Step() {
    float dt = GetFrameTime();
    // Clamp delta-time to avoid spiral of death on lag spikes
    if (dt > 0.05f) dt = 0.05f;

    Update(dt);
    Draw();
}

void Game::Update(float dt) {
    const auto& input = InputManager::Instance();

    // Toggle debug mode on F1
    if (input.IsPressed(GameAction::ToggleDebug)) {
        auto& cfg = GetGameConfig();
        cfg.debugHitboxes = !cfg.debugHitboxes;
    }

    Vector2 virtualMouse = GetVirtualMousePosition();

    // State-specific updates
    switch (m_currentState) {
        case GameStateId::Playing: {
            if (input.IsPressed(GameAction::Pause)) {
                ChangeState(GameStateId::Paused);
                return;
            }

            if (m_player) {
                if (m_player->IsAlive()) {
                    m_player->HandleInput();
                    m_player->Update(dt, m_physicsWorld);
                }

                // Check death or fall into bottom void pit
                if (!m_player->IsAlive() || (m_currentLevel && m_player->GetPosition().y > m_currentLevel->GetPixelHeight() + 24.0f)) {
                    if (m_player->IsAlive()) {
                        m_player->Kill();
                    }
                    ChangeState(GameStateId::GameOver);
                    return;
                }
            }

            if (m_currentLevel) {
                Vector2 playerPos = m_player ? m_player->GetPosition() : Vector2{ 0, 0 };
                m_currentLevel->Update(dt, playerPos);

                for (auto& enemy : m_currentLevel->GetEnemies()) {
                    if (enemy && enemy->IsAlive()) {
                        enemy->Update(dt, m_physicsWorld);
                    }
                }

                if (auto* boss = m_currentLevel->GetBoss()) {
                    boss->Update(dt, m_physicsWorld);
                }
            }

            UpdateCombat(dt);
            UpdateProjectiles(dt);
            CheckTriggers();

            // Verify player alive status after combat/hazard resolution
            if (m_player && !m_player->IsAlive()) {
                ChangeState(GameStateId::GameOver);
                return;
            }

            if (m_player && m_cameraController) {
                m_cameraController->Update(dt, m_player->GetPosition(), m_player->GetFacingDirection());
            }

            ParticleSystem::Instance().Update(dt);
            AudioPlayer::Instance().Update();
            break;
        }

        case GameStateId::Paused:
        case GameStateId::MainMenu:
        case GameStateId::LevelSelect:
        case GameStateId::GameOver:
        case GameStateId::LevelComplete:
        case GameStateId::Victory:
        case GameStateId::Settings: {
            if (m_menuManager) {
                m_menuManager->Update(m_currentState, virtualMouse);
            }
            if (m_currentState == GameStateId::Paused && input.IsPressed(GameAction::Pause)) {
                ChangeState(GameStateId::Playing);
            }
            break;
        }

        default:
            break;
    }
}

void Game::UpdateCombat(float dt) {
    (void)dt;
    if (!m_player || !m_player->IsAlive() || !m_currentLevel) return;

    AABB playerBox = m_player->GetBounds();

    // 1. Player Melee Attack vs Enemies
    auto& blade = m_player->GetEmberBlade();
    if (blade.IsSlashing()) {
        AABB swordHitbox = blade.GetHitbox(m_player->GetPosition(), m_player->GetFacingDirection());

        for (auto& enemy : m_currentLevel->GetEnemies()) {
            if (enemy && enemy->IsAlive()) {
                if (!blade.HasHitEntity(enemy->GetId()) && Intersects(swordHitbox, enemy->GetBounds())) {
                    DamageInfo dmg;
                    dmg.amount = blade.GetDamage();
                    float dirX = (m_player->GetFacingDirection() == Direction::Left) ? -1.0f : 1.0f;
                    dmg.knockback = { dirX * blade.GetKnockback(), -120.0f };
                    dmg.sourcePosition = m_player->GetPosition();
                    dmg.isFromPlayer = true;
                    dmg.attackId = blade.GetCurrentAttackId();

                    if (enemy->TakeDamage(dmg)) {
                        blade.RegisterHitEntity(enemy->GetId());
                        m_currentScore += 50;
                    }
                }
            }
        }

        // Sword vs Boss
        if (auto* boss = m_currentLevel->GetBoss()) {
            if (boss->IsEncounterActive() && boss->IsAlive()) {
                if (!blade.HasHitEntity(boss->GetId()) && Intersects(swordHitbox, boss->GetBounds())) {
                    DamageInfo dmg;
                    dmg.amount = blade.GetDamage();
                    float dirX = (m_player->GetFacingDirection() == Direction::Left) ? -1.0f : 1.0f;
                    dmg.knockback = { dirX * 60.0f, -40.0f };
                    dmg.sourcePosition = m_player->GetPosition();
                    dmg.isFromPlayer = true;
                    dmg.attackId = blade.GetCurrentAttackId();

                    if (boss->TakeDamage(dmg)) {
                        blade.RegisterHitEntity(boss->GetId());
                        m_currentScore += 100;
                    }
                }
            }
        }
    }

    // 2. Enemy Contact Damage against Player
    for (auto& enemy : m_currentLevel->GetEnemies()) {
        if (enemy && enemy->IsAlive()) {
            if (Intersects(playerBox, enemy->GetBounds())) {
                DamageInfo dmg;
                dmg.amount = enemy->GetContactDamage();
                float knockDirX = (m_player->GetPosition().x < enemy->GetPosition().x) ? -1.0f : 1.0f;
                dmg.knockback = { knockDirX * 180.0f, -140.0f };
                dmg.sourcePosition = enemy->GetPosition();
                dmg.isFromPlayer = false;

                m_player->TakeDamage(dmg);
            }
        }
    }

    // Boss Contact Damage
    if (auto* boss = m_currentLevel->GetBoss()) {
        if (boss->IsEncounterActive() && boss->IsAlive()) {
            if (Intersects(playerBox, boss->GetBounds())) {
                DamageInfo dmg;
                dmg.amount = boss->GetContactDamage();
                float knockDirX = (m_player->GetPosition().x < boss->GetPosition().x) ? -1.0f : 1.0f;
                dmg.knockback = { knockDirX * 220.0f, -160.0f };
                dmg.sourcePosition = boss->GetPosition();
                dmg.isFromPlayer = false;

                m_player->TakeDamage(dmg);
            }
        }
    }
}

void Game::UpdateProjectiles(float dt) {
    if (!m_player || !m_currentLevel) return;

    AABB playerBox = m_player->GetBounds();

    for (auto& p : m_projectiles) {
        if (!p.IsActive()) continue;

        p.Update(dt);
        if (!p.IsActive()) continue;

        AABB pBox = p.GetBounds();

        // 1. Collision with solids
        for (const auto& plat : m_physicsWorld.GetPlatforms()) {
            if (!plat.isOneWay && !plat.isHazard && Intersects(pBox, plat.bounds)) {
                p.Deactivate();
                break;
            }
        }
        if (!p.IsActive()) continue;

        // 2. Player Projectile vs Enemies & Boss
        if (p.IsFromPlayer()) {
            for (auto& enemy : m_currentLevel->GetEnemies()) {
                if (enemy && enemy->IsAlive() && Intersects(pBox, enemy->GetBounds())) {
                    DamageInfo dmg;
                    dmg.amount = p.GetDamage();
                    dmg.knockback = { (p.GetVelocity().x > 0 ? 1.0f : -1.0f) * 120.0f, -60.0f };
                    dmg.sourcePosition = p.GetPosition();
                    dmg.isFromPlayer = true;

                    if (enemy->TakeDamage(dmg)) {
                        m_currentScore += 30;
                    }
                    p.Deactivate();
                    break;
                }
            }

            if (p.IsActive()) {
                if (auto* boss = m_currentLevel->GetBoss()) {
                    if (boss->IsEncounterActive() && boss->IsAlive() && Intersects(pBox, boss->GetBounds())) {
                        DamageInfo dmg;
                        dmg.amount = p.GetDamage();
                        dmg.knockback = { (p.GetVelocity().x > 0 ? 1.0f : -1.0f) * 40.0f, 0.0f };
                        dmg.sourcePosition = p.GetPosition();
                        dmg.isFromPlayer = true;

                        if (boss->TakeDamage(dmg)) {
                            m_currentScore += 60;
                        }
                        p.Deactivate();
                    }
                }
            }
        } else {
            // 3. Enemy / Boss Projectile vs Player
            if (m_player->IsAlive() && Intersects(pBox, playerBox)) {
                DamageInfo dmg;
                dmg.amount = p.GetDamage();
                dmg.knockback = { (p.GetVelocity().x > 0 ? 1.0f : -1.0f) * 160.0f, -120.0f };
                dmg.sourcePosition = p.GetPosition();
                dmg.isFromPlayer = false;

                m_player->TakeDamage(dmg);
                p.Deactivate();
            }
        }
    }
}

void Game::CheckTriggers() {
    if (!m_player || !m_player->IsAlive() || !m_currentLevel) return;

    AABB playerBox = m_player->GetBounds();

    // 1. Check Hazards
    for (auto& haz : m_currentLevel->GetHazards()) {
        float dmg = 0.0f;
        if (haz.CheckDamagePlayer(playerBox, dmg)) {
            DamageInfo d;
            d.amount = dmg;
            d.knockback = { 0.0f, -220.0f };
            d.sourcePosition = m_player->GetPosition();
            m_player->TakeDamage(d);
            if (!m_player->IsAlive()) {
                ChangeState(GameStateId::GameOver);
                return;
            }
        }
    }

    // 2. Check Collectibles
    for (auto& col : m_currentLevel->GetCollectibles()) {
        if (col.CheckPickup(playerBox)) {
            if (col.GetType() == CollectibleType::Health) {
                m_player->Heal(35.0f);
            } else if (col.GetType() == CollectibleType::Energy) {
                m_player->RestoreEnergy(40.0f);
            } else if (col.GetType() == CollectibleType::EmberCore) {
                m_collectedCores++;
                SaveSystem::Instance().AddEmberCore();
            }
        }
    }

    // 3. Check Checkpoints
    for (auto& cp : m_currentLevel->GetCheckpoints()) {
        if (cp.CheckActivate(playerBox)) {
            m_player->SetCheckpoint(cp.GetPosition());
        }
    }

    // 4. Boss Arena Trigger (Level 3)
    if (auto* boss = m_currentLevel->GetBoss()) {
        if (!boss->IsEncounterActive() && m_player->GetPosition().x >= 1020.0f) {
            boss->ActivateEncounter();
            m_bossArenaLocked = true;
            // Lock arena boundary in physics world
            m_physicsWorld.AddSolid(AABB{ 984.0f, 0.0f, 24.0f, m_currentLevel->GetPixelHeight() });
            AudioPlayer::Instance().PlaySoundEffect(SoundId::BossRoar);
            ParticleSystem::Instance().TriggerScreenShake(6.0f, 0.4f);
        }

        // Check if boss defeated
        if (boss->IsDefeated()) {
            SaveSystem::Instance().UnlockLevel(3);
            SaveSystem::Instance().AddScore(m_currentScore);
            ChangeState(GameStateId::Victory);
            return;
        }
    }

    // 5. Exit Trigger
    if (Intersects(playerBox, m_currentLevel->GetExitTrigger())) {
        int curLvl = m_currentLevel->GetIndex();
        SaveSystem::Instance().UnlockLevel(curLvl + 1);
        SaveSystem::Instance().AddScore(m_currentScore);

        if (curLvl < LevelManager::Instance().GetTotalLevels()) {
            ChangeState(GameStateId::LevelComplete);
        } else {
            ChangeState(GameStateId::Victory);
        }
    }
}

void Game::HandleMenuAction(const std::string& action, int param) {
    if (action == "play") {
        LoadLevel(param > 0 ? param : 1);
    } else if (action == "level_select") {
        ChangeState(GameStateId::LevelSelect);
    } else if (action == "settings") {
        ChangeState(GameStateId::Settings);
    } else if (action == "resume") {
        ChangeState(GameStateId::Playing);
    } else if (action == "restart") {
        RestartCurrentLevel();
    } else if (action == "retry_checkpoint") {
        RetryFromCheckpoint();
    } else if (action == "next_level") {
        int next = LevelManager::Instance().GetCurrentLevelIndex() + 1;
        LoadLevel(next);
    } else if (action == "main_menu") {
        ChangeState(GameStateId::MainMenu);
    } else if (action == "back") {
        ChangeState(m_previousState == GameStateId::Settings ? GameStateId::MainMenu : m_previousState);
    } else if (action == "quit") {
        m_isRunning = false;
    }
}

Vector2 Game::GetVirtualMousePosition() const {
    Vector2 mouse = ::GetMousePosition();
    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();

    float scale = std::min((float)screenW / m_virtualWidth, (float)screenH / m_virtualHeight);
    float offsetX = (screenW - m_virtualWidth * scale) * 0.5f;
    float offsetY = (screenH - m_virtualHeight * scale) * 0.5f;

    Vector2 virtualMouse;
    virtualMouse.x = (mouse.x - offsetX) / scale;
    virtualMouse.y = (mouse.y - offsetY) / scale;
    return virtualMouse;
}

void Game::Draw() {
    // 1. Render game into virtual target (640x360)
    BeginTextureMode(m_target);
    ClearBackground(Color{ 18, 16, 26, 255 });

    if (m_currentState == GameStateId::Playing || m_currentState == GameStateId::Paused || m_currentState == GameStateId::GameOver) {
        if (m_currentLevel && m_cameraController) {
            Camera2D cam = m_cameraController->GetRaylibCamera();

            // Background with parallax
            m_currentLevel->DrawBackground(cam.target);

            // World space elements
            BeginMode2D(cam);

            m_currentLevel->DrawTiles();
            m_currentLevel->DrawEntities();

            if (m_player) {
                m_player->Draw();
            }

            for (const auto& p : m_projectiles) {
                p.Draw();
            }

            ParticleSystem::Instance().Draw();

            if (GetGameConfig().debugHitboxes) {
                m_physicsWorld.DrawDebug();
            }

            EndMode2D();

            // Screen HUD
            if (m_player) {
                m_hud.Draw(*m_player, m_currentLevel->GetBoss(), m_currentScore, m_collectedCores, m_currentLevel->GetIndex());
            }
        }
    }

    // UI Overlay for menus
    if (m_currentState != GameStateId::Playing && m_menuManager) {
        m_menuManager->Draw(m_currentState);
    }

    EndTextureMode();

    // 2. Render scaled target texture with letterboxing to screen
    BeginDrawing();
    ClearBackground(BLACK);

    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();
    float scale = std::min((float)screenW / m_virtualWidth, (float)screenH / m_virtualHeight);
    float destW = m_virtualWidth * scale;
    float destH = m_virtualHeight * scale;
    float offsetX = (screenW - destW) * 0.5f;
    float offsetY = (screenH - destH) * 0.5f;

    Rectangle src = { 0.0f, 0.0f, (float)m_virtualWidth, -(float)m_virtualHeight }; // Invert Y for OpenGL render texture
    Rectangle dest = { offsetX, offsetY, destW, destH };
    DrawTexturePro(m_target.texture, src, dest, Vector2{ 0, 0 }, 0.0f, WHITE);

    EndDrawing();
}

} // namespace riftbound

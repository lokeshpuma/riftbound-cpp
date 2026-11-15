#include "ui/Menu.hpp"
#include "core/Config.hpp"
#include "core/SaveSystem.hpp"
#include <string>

namespace riftbound {

MenuManager::MenuManager(ActionCallback cb)
    : m_callback(cb) {
    InitMainMenu();
    InitLevelSelect();
    InitPauseMenu();
    InitGameOver();
    InitLevelComplete();
    InitVictory();
    InitSettings();
}

void MenuManager::InitMainMenu() {
    m_mainMenuButtons.clear();
    m_mainMenuButtons.emplace_back(Rectangle{ 250, 150, 140, 32 }, "PLAY GAME");
    m_mainMenuButtons.emplace_back(Rectangle{ 250, 192, 140, 32 }, "LEVEL SELECT");
    m_mainMenuButtons.emplace_back(Rectangle{ 250, 234, 140, 32 }, "SETTINGS");
    m_mainMenuButtons.emplace_back(Rectangle{ 250, 276, 140, 32 }, "QUIT");
}

void MenuManager::InitLevelSelect() {
    m_levelSelectButtons.clear();
    m_levelSelectButtons.emplace_back(Rectangle{ 210, 90, 220, 26 }, "LEVEL 1: RUINS");
    m_levelSelectButtons.emplace_back(Rectangle{ 210, 124, 220, 26 }, "LEVEL 2: SPIRE");
    m_levelSelectButtons.emplace_back(Rectangle{ 210, 158, 220, 26 }, "LEVEL 3: CATACOMBS");
    m_levelSelectButtons.emplace_back(Rectangle{ 210, 192, 220, 26 }, "LEVEL 4: RAMPARTS");
    m_levelSelectButtons.emplace_back(Rectangle{ 210, 226, 220, 26 }, "LEVEL 5: CRUCIBLE (BOSS)");
    m_levelSelectButtons.emplace_back(Rectangle{ 210, 270, 220, 28 }, "BACK TO MENU");
}

void MenuManager::InitPauseMenu() {
    m_pauseButtons.clear();
    m_pauseButtons.emplace_back(Rectangle{ 250, 130, 140, 30 }, "RESUME");
    m_pauseButtons.emplace_back(Rectangle{ 250, 170, 140, 30 }, "RESTART LEVEL");
    m_pauseButtons.emplace_back(Rectangle{ 250, 210, 140, 30 }, "SETTINGS");
    m_pauseButtons.emplace_back(Rectangle{ 250, 250, 140, 30 }, "MAIN MENU");
}

void MenuManager::InitGameOver() {
    m_gameOverButtons.clear();
    m_gameOverButtons.emplace_back(Rectangle{ 230, 180, 180, 32 }, "RETRY CHECKPOINT");
    m_gameOverButtons.emplace_back(Rectangle{ 230, 222, 180, 32 }, "RESTART LEVEL");
    m_gameOverButtons.emplace_back(Rectangle{ 230, 264, 180, 32 }, "MAIN MENU");
}

void MenuManager::InitLevelComplete() {
    m_levelCompleteButtons.clear();
    m_levelCompleteButtons.emplace_back(Rectangle{ 240, 180, 160, 32 }, "NEXT LEVEL");
    m_levelCompleteButtons.emplace_back(Rectangle{ 240, 222, 160, 32 }, "LEVEL SELECT");
    m_levelCompleteButtons.emplace_back(Rectangle{ 240, 264, 160, 32 }, "MAIN MENU");
}

void MenuManager::InitVictory() {
    m_victoryButtons.clear();
    m_victoryButtons.emplace_back(Rectangle{ 230, 240, 180, 34 }, "RETURN TO MENU");
}

void MenuManager::InitSettings() {
    const auto& cfg = GetGameConfig();
    m_masterSlider = std::make_unique<UISlider>(Rectangle{ 240, 120, 160, 12 }, "Master Volume", 0.0f, 1.0f, cfg.masterVolume);
    m_musicSlider = std::make_unique<UISlider>(Rectangle{ 240, 160, 160, 12 }, "Music Volume", 0.0f, 1.0f, cfg.musicVolume);
    m_sfxSlider = std::make_unique<UISlider>(Rectangle{ 240, 200, 160, 12 }, "SFX Volume", 0.0f, 1.0f, cfg.sfxVolume);

    m_settingsButtons.clear();
    m_settingsButtons.emplace_back(Rectangle{ 240, 230, 160, 26 }, cfg.screenShakeEnabled ? "Screen Shake: ON" : "Screen Shake: OFF");
    m_settingsButtons.emplace_back(Rectangle{ 240, 262, 160, 26 }, cfg.debugHitboxes ? "Debug Hitboxes: ON" : "Debug Hitboxes: OFF");
    m_settingsButtons.emplace_back(Rectangle{ 240, 296, 160, 26 }, "BACK");
}

void MenuManager::Update(GameStateId state, Vector2 mousePos) {
    if (!m_callback) return;

    if (state != m_lastState) {
        m_selectedIndex = 0;
        m_lastState = state;
    }

    bool keyUp = IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W);
    bool keyDown = IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S);
    bool keySelect = IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_E);

    auto updateButtonGroup = [&](std::vector<UIButton>& buttons, auto onAction) {
        if (buttons.empty()) return;

        if (keyDown) {
            m_selectedIndex = (m_selectedIndex + 1) % buttons.size();
        } else if (keyUp) {
            m_selectedIndex = (m_selectedIndex - 1 + buttons.size()) % buttons.size();
        }

        for (size_t i = 0; i < buttons.size(); ++i) {
            buttons[i].Update(mousePos);
            if (CheckCollisionPointRec(mousePos, buttons[i].GetBounds())) {
                m_selectedIndex = static_cast<int>(i);
            }
            buttons[i].SetSelected(static_cast<int>(i) == m_selectedIndex);

            if (buttons[i].IsClicked() || (static_cast<int>(i) == m_selectedIndex && keySelect)) {
                onAction(i);
                return;
            }
        }
    };

    if (state == GameStateId::MainMenu) {
        updateButtonGroup(m_mainMenuButtons, [this](size_t i) {
            if (i == 0) m_callback("play", 1);
            else if (i == 1) m_callback("level_select", 0);
            else if (i == 2) m_callback("settings", 0);
            else if (i == 3) m_callback("quit", 0);
        });
    } else if (state == GameStateId::LevelSelect) {
        int unlocked = SaveSystem::Instance().GetData().highestUnlockedLevel;
        updateButtonGroup(m_levelSelectButtons, [this, unlocked](size_t i) {
            if (i < 5) {
                int targetLvl = static_cast<int>(i + 1);
                if (targetLvl <= unlocked) {
                    m_callback("play", targetLvl);
                }
            } else if (i == 5) {
                m_callback("main_menu", 0);
            }
        });
    } else if (state == GameStateId::Paused) {
        updateButtonGroup(m_pauseButtons, [this](size_t i) {
            if (i == 0) m_callback("resume", 0);
            else if (i == 1) m_callback("restart", 0);
            else if (i == 2) m_callback("settings", 0);
            else if (i == 3) m_callback("main_menu", 0);
        });
    } else if (state == GameStateId::GameOver) {
        updateButtonGroup(m_gameOverButtons, [this](size_t i) {
            if (i == 0) m_callback("retry_checkpoint", 0);
            else if (i == 1) m_callback("restart", 0);
            else if (i == 2) m_callback("main_menu", 0);
        });
    } else if (state == GameStateId::LevelComplete) {
        updateButtonGroup(m_levelCompleteButtons, [this](size_t i) {
            if (i == 0) m_callback("next_level", 0);
            else if (i == 1) m_callback("level_select", 0);
            else if (i == 2) m_callback("main_menu", 0);
        });
    } else if (state == GameStateId::Victory) {
        updateButtonGroup(m_victoryButtons, [this](size_t) {
            m_callback("main_menu", 0);
        });
    } else if (state == GameStateId::Settings) {
        auto& cfg = GetGameConfig();
        if (m_masterSlider) {
            m_masterSlider->Update(mousePos);
            cfg.masterVolume = m_masterSlider->GetValue();
        }
        if (m_musicSlider) {
            m_musicSlider->Update(mousePos);
            cfg.musicVolume = m_musicSlider->GetValue();
        }
        if (m_sfxSlider) {
            m_sfxSlider->Update(mousePos);
            cfg.sfxVolume = m_sfxSlider->GetValue();
        }

        updateButtonGroup(m_settingsButtons, [this, &cfg](size_t i) {
            if (i == 0) {
                cfg.screenShakeEnabled = !cfg.screenShakeEnabled;
                InitSettings();
            } else if (i == 1) {
                cfg.debugHitboxes = !cfg.debugHitboxes;
                InitSettings();
            } else if (i == 2) {
                SaveSystem::Instance().Save();
                m_callback("back", 0);
            }
        });
    }
}

void MenuManager::Draw(GameStateId state) const {
    if (state == GameStateId::MainMenu) {
        // Backdrop overlay
        DrawRectangle(0, 0, 640, 360, Color{ 16, 14, 24, 255 });

        // Title
        DrawText("RIFTBOUND", 200, 50, 42, Color{ 255, 170, 40, 255 });
        DrawText("Echoes of Ember", 244, 98, 18, Color{ 255, 220, 150, 255 });

        for (const auto& btn : m_mainMenuButtons) {
            btn.Draw();
        }

        DrawControlsOverlay();
    } else if (state == GameStateId::LevelSelect) {
        DrawRectangle(0, 0, 640, 360, Color{ 16, 14, 24, 255 });
        DrawText("LEVEL SELECT", 250, 45, 24, GOLD);

        int unlocked = SaveSystem::Instance().GetData().highestUnlockedLevel;
        for (size_t i = 0; i < m_levelSelectButtons.size(); ++i) {
            m_levelSelectButtons[i].Draw();
            if (i < 5 && static_cast<int>(i + 1) > unlocked) {
                DrawText("[LOCKED]", 440, static_cast<int>(96 + i * 34), 11, RED);
            }
        }
    } else if (state == GameStateId::Paused) {
        // Semi-transparent overlay
        DrawRectangle(0, 0, 640, 360, Color{ 10, 10, 15, 180 });
        DrawText("GAME PAUSED", 252, 75, 22, WHITE);

        for (const auto& btn : m_pauseButtons) {
            btn.Draw();
        }

        DrawControlsOverlay();
    } else if (state == GameStateId::GameOver) {
        DrawRectangle(0, 0, 640, 360, Color{ 40, 10, 15, 210 });
        DrawText("EMBER EXTINGUISHED", 175, 80, 26, RED);
        DrawText("You were consumed by the rift hazards.", 205, 120, 13, LIGHTGRAY);

        for (const auto& btn : m_gameOverButtons) {
            btn.Draw();
        }
    } else if (state == GameStateId::LevelComplete) {
        DrawRectangle(0, 0, 640, 360, Color{ 15, 25, 35, 210 });
        DrawText("STAGE CLEARED!", 220, 90, 28, GOLD);
        DrawText("All threats in this sector overcome.", 205, 135, 14, SKYBLUE);

        for (const auto& btn : m_levelCompleteButtons) {
            btn.Draw();
        }
    } else if (state == GameStateId::Victory) {
        DrawRectangle(0, 0, 640, 360, Color{ 20, 20, 35, 240 });
        DrawText("VICTORY ACHIEVED!", 185, 70, 32, GOLD);
        DrawText("The Ashen Warden has fallen. Peace returns to the Rift.", 145, 120, 14, WHITE);

        int cores = SaveSystem::Instance().GetData().emberCoresCollected;
        int score = SaveSystem::Instance().GetData().totalScore;
        std::string statStr = "Total Ember Cores: " + std::to_string(cores) + "   |   Final Score: " + std::to_string(score);
        DrawText(statStr.c_str(), 175, 170, 14, Color{ 255, 210, 50, 255 });

        for (const auto& btn : m_victoryButtons) {
            btn.Draw();
        }
    } else if (state == GameStateId::Settings) {
        DrawRectangle(0, 0, 640, 360, Color{ 16, 14, 24, 255 });
        DrawText("SETTINGS", 270, 60, 24, GOLD);

        if (m_masterSlider) m_masterSlider->Draw();
        if (m_musicSlider) m_musicSlider->Draw();
        if (m_sfxSlider) m_sfxSlider->Draw();

        for (const auto& btn : m_settingsButtons) {
            btn.Draw();
        }
    }
}

void MenuManager::DrawControlsOverlay() const {
    DrawRectangle(30, 316, 580, 32, Color{ 25, 22, 35, 200 });
    DrawRectangleLinesEx(Rectangle{ 30, 316, 580, 32 }, 1.0f, Color{ 60, 55, 80, 255 });
    DrawText("CONTROLS: A/D/Arrows: Move | Space/W: Jump | J/LMB: Sword | K/RMB: Pulse | Q: Swap Weapon | Esc: Pause", 42, 326, 9, Color{ 200, 205, 225, 255 });
}

} // namespace riftbound

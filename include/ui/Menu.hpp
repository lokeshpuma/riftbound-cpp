#pragma once

#include "ui/UIElements.hpp"
#include "core/Types.hpp"
#include <vector>
#include <memory>
#include <functional>

namespace riftbound {

class MenuManager {
public:
    using ActionCallback = std::function<void(const std::string& action, int param)>;

    MenuManager(ActionCallback cb = nullptr);

    void SetCallback(ActionCallback cb) { m_callback = cb; }

    void Update(GameStateId state, Vector2 mousePos);
    void Draw(GameStateId state) const;

    void DrawControlsOverlay() const;

private:
    ActionCallback m_callback;
    int m_selectedIndex = 0;
    GameStateId m_lastState = GameStateId::MainMenu;

    // Buttons for various screens
    std::vector<UIButton> m_mainMenuButtons;
    std::vector<UIButton> m_levelSelectButtons;
    std::vector<UIButton> m_pauseButtons;
    std::vector<UIButton> m_gameOverButtons;
    std::vector<UIButton> m_levelCompleteButtons;
    std::vector<UIButton> m_victoryButtons;
    std::vector<UIButton> m_settingsButtons;

    // Sliders for Settings
    std::unique_ptr<UISlider> m_masterSlider;
    std::unique_ptr<UISlider> m_musicSlider;
    std::unique_ptr<UISlider> m_sfxSlider;

    void InitMainMenu();
    void InitLevelSelect();
    void InitPauseMenu();
    void InitGameOver();
    void InitLevelComplete();
    void InitVictory();
    void InitSettings();
};

} // namespace riftbound

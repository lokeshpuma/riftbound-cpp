#pragma once

#include "entities/Player.hpp"
#include "entities/Boss.hpp"
#include <raylib.h>

namespace riftbound {

class HUD {
public:
    HUD() = default;

    void Draw(const Player& player, const AshenWarden* boss, int score, int cores, int currentLevel) const;

private:
    void DrawPlayerStats(const Player& player) const;
    void DrawWeaponIndicator(const Player& player) const;
    void DrawScoreAndLevel(int score, int cores, int currentLevel) const;
    void DrawBossBar(const AshenWarden& boss) const;
};

} // namespace riftbound

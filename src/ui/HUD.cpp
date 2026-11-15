#include "ui/HUD.hpp"
#include <string>
#include <cmath>

namespace riftbound {

void HUD::Draw(const Player& player, const AshenWarden* boss, int score, int cores, int currentLevel) const {
    DrawPlayerStats(player);
    DrawWeaponIndicator(player);
    DrawScoreAndLevel(score, cores, currentLevel);

    if (boss && boss->IsEncounterActive() && boss->IsAlive()) {
        DrawBossBar(*boss);
    }
}

void HUD::DrawPlayerStats(const Player& player) const {
    // Health Bar Frame
    DrawRectangle(14, 14, 144, 18, Color{ 25, 25, 35, 220 });
    DrawRectangleLinesEx(Rectangle{ 14, 14, 144, 18 }, 1.0f, Color{ 70, 75, 95, 255 });

    float hpPct = player.GetHealth() / player.GetMaxHealth();
    Color hpCol = (hpPct > 0.35f) ? Color{ 50, 205, 80, 255 } : Color{ 230, 40, 40, 255 };
    DrawRectangle(16, 16, static_cast<int>(140.0f * hpPct), 14, hpCol);

    std::string hpText = "HP: " + std::to_string(static_cast<int>(player.GetHealth())) + " / " + std::to_string(static_cast<int>(player.GetMaxHealth()));
    DrawText(hpText.c_str(), 20, 18, 10, WHITE);

    // Energy Bar Frame
    DrawRectangle(14, 34, 114, 12, Color{ 25, 25, 35, 220 });
    DrawRectangleLinesEx(Rectangle{ 14, 34, 114, 12 }, 1.0f, Color{ 70, 75, 95, 255 });

    float nrgPct = player.GetEnergy() / player.GetMaxEnergy();
    DrawRectangle(16, 36, static_cast<int>(110.0f * nrgPct), 8, Color{ 40, 190, 240, 255 });

    std::string nrgText = "ENERGY: " + std::to_string(static_cast<int>(player.GetEnergy()));
    DrawText(nrgText.c_str(), 20, 36, 8, WHITE);
}

void HUD::DrawWeaponIndicator(const Player& player) const {
    // Weapon Frame
    DrawRectangle(14, 50, 114, 20, Color{ 30, 28, 42, 220 });
    DrawRectangleLinesEx(Rectangle{ 14, 50, 114, 20 }, 1.0f, Color{ 90, 85, 110, 255 });

    if (player.GetCurrentWeaponType() == WeaponType::EmberBlade) {
        DrawCircle(24, 60, 5, Color{ 255, 140, 30, 255 });
        DrawText("EMBER BLADE [Q]", 34, 55, 9, Color{ 255, 190, 80, 255 });
    } else {
        DrawCircle(24, 60, 5, Color{ 60, 210, 255, 255 });
        DrawText("PULSE BOLTS [Q]", 34, 55, 9, Color{ 120, 230, 255, 255 });
    }
}

void HUD::DrawScoreAndLevel(int score, int cores, int currentLevel) const {
    // Top right indicators
    std::string lvlStr = "STAGE " + std::to_string(currentLevel);
    DrawText(lvlStr.c_str(), 560, 16, 12, GOLD);

    std::string scoreStr = "SCORE: " + std::to_string(score);
    DrawText(scoreStr.c_str(), 540, 32, 11, WHITE);

    // Ember Core count
    DrawCircle(545, 52, 4, Color{ 255, 180, 20, 255 });
    std::string coreStr = "x " + std::to_string(cores);
    DrawText(coreStr.c_str(), 556, 48, 11, YELLOW);
}

void HUD::DrawBossBar(const AshenWarden& boss) const {
    // Centered at top of screen
    int barW = 320;
    int barH = 14;
    int barX = (640 - barW) / 2;
    int barY = 24;

    DrawRectangle(barX - 2, barY - 14, barW + 4, 32, Color{ 20, 16, 28, 230 });
    DrawRectangleLinesEx(Rectangle{ (float)(barX - 2), (float)(barY - 14), (float)(barW + 4), 32.0f }, 1.0f, Color{ 180, 50, 40, 255 });

    DrawText("THE ASHEN WARDEN", barX + 10, barY - 12, 10, Color{ 255, 180, 50, 255 });

    std::string phaseStr = "PHASE 1";
    if (boss.GetPhase() == BossPhase::Phase2) phaseStr = "PHASE 2";
    else if (boss.GetPhase() == BossPhase::Phase3) phaseStr = "PHASE 3: ENRAGED!";
    DrawText(phaseStr.c_str(), barX + barW - 110, barY - 12, 10, RED);

    // Bar background & fill
    DrawRectangle(barX, barY, barW, barH, Color{ 40, 30, 40, 255 });
    float hpPct = boss.GetHealth() / boss.GetMaxHealth();
    DrawRectangle(barX, barY, static_cast<int>(barW * hpPct), barH, Color{ 220, 40, 30, 255 });
}

} // namespace riftbound

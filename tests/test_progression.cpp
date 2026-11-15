#include "core/SaveSystem.hpp"
#include "level/LevelManager.hpp"
#include <iostream>

extern int g_totalTests;
extern int g_passedTests;

#define EXPECT_TRUE(cond) do { \
    g_totalTests++; \
    if (cond) { \
        g_passedTests++; \
    } else { \
        std::cerr << "FAILED: " << #cond << " at " << __FILE__ << ":" << __LINE__ << "\n"; \
    } \
} while (0)

#define EXPECT_FALSE(cond) EXPECT_TRUE(!(cond))

void RunProgressionTests() {
    std::cout << "[Progression] Running save & level progression tests...\n";

    using namespace riftbound;

    // Test 1: Save System defaults & reset
    {
        auto& save = SaveSystem::Instance();
        save.ResetToDefaults();
        EXPECT_TRUE(save.GetData().highestUnlockedLevel == 1);
        EXPECT_TRUE(save.GetData().totalScore == 0);
        EXPECT_TRUE(save.GetData().emberCoresCollected == 0);

        save.UnlockLevel(2);
        EXPECT_TRUE(save.GetData().highestUnlockedLevel == 2);

        // Cannot regress to lower level
        save.UnlockLevel(1);
        EXPECT_TRUE(save.GetData().highestUnlockedLevel == 2);

        save.AddScore(500);
        EXPECT_TRUE(save.GetData().totalScore == 500);

        save.AddEmberCore();
        EXPECT_TRUE(save.GetData().emberCoresCollected == 1);
        EXPECT_TRUE(save.GetData().totalScore == 750); // 500 + 250
    }

    // Test 2: Level Manager layout verification
    {
        auto& mgr = LevelManager::Instance();
        mgr.Init();
        EXPECT_TRUE(mgr.GetTotalLevels() == 3);

        auto lvl1 = mgr.LoadLevel(1);
        EXPECT_TRUE(lvl1 != nullptr);
        EXPECT_TRUE(lvl1->GetIndex() == 1);
        EXPECT_TRUE(lvl1->GetEnemies().size() >= 3);
        EXPECT_TRUE(lvl1->GetCheckpoints().size() >= 1);
        EXPECT_TRUE(lvl1->GetCollectibles().size() >= 3);

        auto lvl2 = mgr.LoadLevel(2);
        EXPECT_TRUE(lvl2 != nullptr);
        EXPECT_TRUE(lvl2->GetIndex() == 2);
        EXPECT_TRUE(lvl2->GetHazards().size() >= 1);
        EXPECT_TRUE(lvl2->GetEnemies().size() >= 4);

        auto lvl3 = mgr.LoadLevel(3);
        EXPECT_TRUE(lvl3 != nullptr);
        EXPECT_TRUE(lvl3->GetIndex() == 3);
        EXPECT_TRUE(lvl3->GetBoss() != nullptr);
        EXPECT_TRUE(lvl3->GetBoss()->GetMaxHealth() >= 350.0f);
    }

    std::cout << "[Progression] Tests completed.\n";
}

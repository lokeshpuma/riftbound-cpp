#pragma once

#include "level/Level.hpp"
#include <memory>
#include <string>

namespace riftbound {

class LevelManager {
public:
    static LevelManager& Instance();

    void Init();
    std::unique_ptr<Level> LoadLevel(int levelIndex);

    int GetCurrentLevelIndex() const { return m_currentLevelIndex; }
    void SetCurrentLevelIndex(int idx) { m_currentLevelIndex = idx; }

    int GetTotalLevels() const { return 3; }
    std::string GetLevelName(int levelIndex) const;

private:
    LevelManager() = default;

    int m_currentLevelIndex = 1;

    std::unique_ptr<Level> BuildLevel1();
    std::unique_ptr<Level> BuildLevel2();
    std::unique_ptr<Level> BuildLevel3();
};

} // namespace riftbound

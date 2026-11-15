#pragma once

#include <string>

namespace riftbound {

struct SaveData {
    int highestUnlockedLevel = 1;
    int totalScore = 0;
    int emberCoresCollected = 0;
    float masterVolume = 0.8f;
    float musicVolume = 0.6f;
    float sfxVolume = 0.8f;
    bool screenShakeEnabled = true;
};

class SaveSystem {
public:
    static SaveSystem& Instance();

    void Load();
    void Save();
    void ResetToDefaults();

    SaveData& GetData() { return m_data; }
    const SaveData& GetData() const { return m_data; }

    void UnlockLevel(int levelIndex);
    void AddScore(int points);
    void AddEmberCore();

private:
    SaveSystem();
    std::string m_saveFilePath;
    SaveData m_data;
};

} // namespace riftbound

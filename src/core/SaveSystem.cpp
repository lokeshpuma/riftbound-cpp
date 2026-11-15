#include "core/SaveSystem.hpp"
#include "core/Config.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <string>

#if defined(__EMSCRIPTEN__)
#include <emscripten.h>
#endif

namespace riftbound {

SaveSystem::SaveSystem() {
    m_saveFilePath = "riftbound_save.dat";
}

SaveSystem& SaveSystem::Instance() {
    static SaveSystem instance;
    return instance;
}

void SaveSystem::ResetToDefaults() {
    m_data = SaveData{};
    auto& cfg = GetGameConfig();
    cfg.masterVolume = m_data.masterVolume;
    cfg.musicVolume = m_data.musicVolume;
    cfg.sfxVolume = m_data.sfxVolume;
    cfg.screenShakeEnabled = m_data.screenShakeEnabled;
}

void SaveSystem::Load() {
    std::ifstream file(m_saveFilePath);
    if (!file.is_open()) {
        std::cout << "[SaveSystem] No save file found. Initializing with defaults.\n";
        ResetToDefaults();
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;
        std::istringstream iss(line);
        std::string key;
        if (std::getline(iss, key, '=')) {
            std::string value;
            if (std::getline(iss, value)) {
                try {
                    if (key == "highestUnlockedLevel") {
                        m_data.highestUnlockedLevel = std::max(1, std::stoi(value));
                    } else if (key == "totalScore") {
                        m_data.totalScore = std::max(0, std::stoi(value));
                    } else if (key == "emberCoresCollected") {
                        m_data.emberCoresCollected = std::max(0, std::stoi(value));
                    } else if (key == "masterVolume") {
                        m_data.masterVolume = std::clamp(std::stof(value), 0.0f, 1.0f);
                    } else if (key == "musicVolume") {
                        m_data.musicVolume = std::clamp(std::stof(value), 0.0f, 1.0f);
                    } else if (key == "sfxVolume") {
                        m_data.sfxVolume = std::clamp(std::stof(value), 0.0f, 1.0f);
                    } else if (key == "screenShakeEnabled") {
                        m_data.screenShakeEnabled = (value == "1" || value == "true");
                    }
                } catch (...) {
                    std::cerr << "[SaveSystem] Warning: Failed to parse save key: " << key << "\n";
                }
            }
        }
    }

    auto& cfg = GetGameConfig();
    cfg.masterVolume = m_data.masterVolume;
    cfg.musicVolume = m_data.musicVolume;
    cfg.sfxVolume = m_data.sfxVolume;
    cfg.screenShakeEnabled = m_data.screenShakeEnabled;

    std::cout << "[SaveSystem] Loaded successfully. Unlocked level: " << m_data.highestUnlockedLevel << "\n";
}

void SaveSystem::Save() {
    const auto& cfg = GetGameConfig();
    m_data.masterVolume = cfg.masterVolume;
    m_data.musicVolume = cfg.musicVolume;
    m_data.sfxVolume = cfg.sfxVolume;
    m_data.screenShakeEnabled = cfg.screenShakeEnabled;

    std::ofstream file(m_saveFilePath, std::ios::trunc);
    if (!file.is_open()) {
        std::cerr << "[SaveSystem] Error: Could not open file for writing: " << m_saveFilePath << "\n";
        return;
    }

    file << "# Riftbound Save Data\n";
    file << "highestUnlockedLevel=" << m_data.highestUnlockedLevel << "\n";
    file << "totalScore=" << m_data.totalScore << "\n";
    file << "emberCoresCollected=" << m_data.emberCoresCollected << "\n";
    file << "masterVolume=" << m_data.masterVolume << "\n";
    file << "musicVolume=" << m_data.musicVolume << "\n";
    file << "sfxVolume=" << m_data.sfxVolume << "\n";
    file << "screenShakeEnabled=" << (m_data.screenShakeEnabled ? "1" : "0") << "\n";

    file.close();

#if defined(__EMSCRIPTEN__)
    EM_ASM(
        if (typeof FS !== 'undefined' && FS.syncfs) {
            FS.syncfs(false, function (err) {
                if (err) console.error("Error syncing IDBFS:", err);
            });
        }
    );
#endif

    std::cout << "[SaveSystem] Progress saved.\n";
}

void SaveSystem::UnlockLevel(int levelIndex) {
    if (levelIndex > m_data.highestUnlockedLevel) {
        m_data.highestUnlockedLevel = levelIndex;
        Save();
    }
}

void SaveSystem::AddScore(int points) {
    m_data.totalScore += points;
}

void SaveSystem::AddEmberCore() {
    m_data.emberCoresCollected++;
    m_data.totalScore += 250;
    Save();
}

} // namespace riftbound

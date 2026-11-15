#pragma once

#include <raylib.h>
#include <string>
#include <unordered_map>

namespace riftbound {

enum class SoundId {
    Jump,
    MeleeSlash,
    RangedFire,
    Impact,
    PlayerHurt,
    EnemyHurt,
    PickupHealth,
    PickupEnergy,
    PickupCore,
    Checkpoint,
    BossSlam,
    BossRoar,
    Victory,
    GameOver
};

class AudioPlayer {
public:
    static AudioPlayer& Instance();

    void Init();
    void Shutdown();

    void PlaySoundEffect(SoundId id);
    void PlayMusicTrack(int trackId);
    void StopMusic();
    void Update();

    void SetMasterVolume(float vol);
    void SetMusicVolume(float vol);
    void SetSfxVolume(float vol);

private:
    AudioPlayer() = default;
    ~AudioPlayer() = default;

    bool m_initialized = false;
    std::unordered_map<int, Sound> m_sounds;
    Music m_currentMusic{};
    bool m_hasActiveMusic = false;
    int m_currentTrackId = -1;

    void SynthesizeFallbackSounds();
    Sound GenerateWaveformSound(int waveType, float baseFreq, float endFreq, float duration, float decay);
};

} // namespace riftbound

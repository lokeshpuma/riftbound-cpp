#include "core/AudioPlayer.hpp"
#include "core/Config.hpp"
#include <cmath>
#include <cstdlib>
#include <iostream>

namespace riftbound {

AudioPlayer& AudioPlayer::Instance() {
    static AudioPlayer instance;
    return instance;
}

void AudioPlayer::Init() {
    if (m_initialized) return;

    InitAudioDevice();
    if (!IsAudioDeviceReady()) {
        std::cerr << "[AudioPlayer] Warning: Audio device could not be initialized.\n";
        return;
    }

    m_initialized = true;
    const auto& cfg = GetGameConfig();
    SetMasterVolume(cfg.masterVolume);

    SynthesizeFallbackSounds();
    std::cout << "[AudioPlayer] Audio device initialized and procedural sounds synthesized.\n";
}

void AudioPlayer::Shutdown() {
    if (!m_initialized) return;

    if (m_hasActiveMusic) {
        StopMusic();
    }

    for (auto& [id, sound] : m_sounds) {
        UnloadSound(sound);
    }
    m_sounds.clear();

    CloseAudioDevice();
    m_initialized = false;
}

Sound AudioPlayer::GenerateWaveformSound(int waveType, float baseFreq, float endFreq, float duration, float decay) {
    const int sampleRate = 44100;
    int frameCount = static_cast<int>(duration * sampleRate);
    short* samples = new short[frameCount];

    double phase = 0.0;
    for (int i = 0; i < frameCount; ++i) {
        float t = static_cast<float>(i) / frameCount;
        float currentFreq = baseFreq + (endFreq - baseFreq) * t;
        double phaseIncrement = 2.0 * 3.141592653589793 * currentFreq / sampleRate;
        phase += phaseIncrement;

        float sample = 0.0f;
        if (waveType == 0) { // Sine wave
            sample = std::sin(phase);
        } else if (waveType == 1) { // Square wave
            sample = (std::sin(phase) >= 0.0) ? 0.7f : -0.7f;
        } else if (waveType == 2) { // Triangle wave
            sample = (2.0f / 3.14159265f) * std::asin(std::sin(phase));
        } else if (waveType == 3) { // Noise
            sample = (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * 2.0f - 1.0f;
        }

        // Apply ADSR-like volume envelope
        float envelope = 1.0f;
        if (decay > 0.0f) {
            envelope = std::exp(-decay * t);
        }
        samples[i] = static_cast<short>(sample * envelope * 24000.0f);
    }

    Wave wave{};
    wave.frameCount = frameCount;
    wave.sampleRate = sampleRate;
    wave.sampleSize = 16;
    wave.channels = 1;
    wave.data = samples;

    Sound snd = LoadSoundFromWave(wave);
    delete[] samples;
    return snd;
}

void AudioPlayer::SynthesizeFallbackSounds() {
    // Jump: rising square / sine chirp
    m_sounds[static_cast<int>(SoundId::Jump)] = GenerateWaveformSound(0, 220.0f, 600.0f, 0.15f, 6.0f);

    // Melee Slash: quick white noise sweep
    m_sounds[static_cast<int>(SoundId::MeleeSlash)] = GenerateWaveformSound(3, 800.0f, 200.0f, 0.12f, 12.0f);

    // Ranged Fire: fast laser blip
    m_sounds[static_cast<int>(SoundId::RangedFire)] = GenerateWaveformSound(1, 750.0f, 250.0f, 0.10f, 10.0f);

    // Impact: low-frequency hit thud
    m_sounds[static_cast<int>(SoundId::Impact)] = GenerateWaveformSound(2, 180.0f, 60.0f, 0.14f, 14.0f);

    // Player Hurt: crunchy downward square buzz
    m_sounds[static_cast<int>(SoundId::PlayerHurt)] = GenerateWaveformSound(1, 300.0f, 90.0f, 0.22f, 8.0f);

    // Enemy Hurt: higher pitch impact
    m_sounds[static_cast<int>(SoundId::EnemyHurt)] = GenerateWaveformSound(2, 440.0f, 150.0f, 0.12f, 10.0f);

    // Pickup Health: bright pleasant ping
    m_sounds[static_cast<int>(SoundId::PickupHealth)] = GenerateWaveformSound(0, 523.0f, 784.0f, 0.20f, 5.0f);

    // Pickup Energy: harmonic chime
    m_sounds[static_cast<int>(SoundId::PickupEnergy)] = GenerateWaveformSound(0, 659.0f, 987.0f, 0.20f, 6.0f);

    // Pickup Core: glorious reward chime
    m_sounds[static_cast<int>(SoundId::PickupCore)] = GenerateWaveformSound(0, 440.0f, 880.0f, 0.35f, 3.5f);

    // Checkpoint: resonant dual tone
    m_sounds[static_cast<int>(SoundId::Checkpoint)] = GenerateWaveformSound(2, 330.0f, 660.0f, 0.40f, 4.0f);

    // Boss Slam: deep explosive boom
    m_sounds[static_cast<int>(SoundId::BossSlam)] = GenerateWaveformSound(3, 200.0f, 40.0f, 0.45f, 5.0f);

    // Boss Roar: menacing low modulated tone
    m_sounds[static_cast<int>(SoundId::BossRoar)] = GenerateWaveformSound(1, 140.0f, 70.0f, 0.60f, 2.5f);

    // Victory: triumphant ascending fanfare
    m_sounds[static_cast<int>(SoundId::Victory)] = GenerateWaveformSound(0, 440.0f, 1046.0f, 0.8f, 2.0f);

    // GameOver: descending somber tone
    m_sounds[static_cast<int>(SoundId::GameOver)] = GenerateWaveformSound(1, 300.0f, 75.0f, 0.7f, 3.0f);
}

void AudioPlayer::PlaySoundEffect(SoundId id) {
    if (!m_initialized) return;
    int key = static_cast<int>(id);
    auto it = m_sounds.find(key);
    if (it != m_sounds.end()) {
        const auto& cfg = GetGameConfig();
        SetSoundVolume(it->second, cfg.sfxVolume * cfg.masterVolume);
        PlaySound(it->second);
    }
}

void AudioPlayer::PlayMusicTrack(int trackId) {
    if (!m_initialized) return;
    if (m_currentTrackId == trackId && m_hasActiveMusic) return;
    m_currentTrackId = trackId;
    // Raylib music stream placeholder or file loader
}

void AudioPlayer::StopMusic() {
    if (m_hasActiveMusic) {
        StopMusicStream(m_currentMusic);
        m_hasActiveMusic = false;
    }
}

void AudioPlayer::Update() {
    if (m_initialized && m_hasActiveMusic) {
        UpdateMusicStream(m_currentMusic);
    }
}

void AudioPlayer::SetMasterVolume(float vol) {
    if (m_initialized) {
        ::SetMasterVolume(vol);
    }
}

void AudioPlayer::SetMusicVolume(float vol) {
    if (m_initialized && m_hasActiveMusic) {
        const auto& cfg = GetGameConfig();
        ::SetMusicVolume(m_currentMusic, vol * cfg.masterVolume);
    }
}

void AudioPlayer::SetSfxVolume(float /*vol*/) {
    // Sfx volume is applied per PlaySoundEffect call
}

} // namespace riftbound

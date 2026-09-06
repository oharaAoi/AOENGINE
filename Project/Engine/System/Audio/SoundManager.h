#pragma once
#include "Audio.h"
#include "SoundCatalog.h"
#include <array>
#include <cstdint>
#include <memory>

namespace AOENGINE {
struct SoundHandle {
    uint32_t index = UINT32_MAX;
    uint64_t generation = 0;
    explicit operator bool() const { return index != UINT32_MAX && generation != 0; }
};
enum class SoundScope { Game, Preview };

// Main-thread API. Instances keep decoded PCM alive until their voice is destroyed.
class SoundManager final {
public:
    explicit SoundManager(Audio& audio, std::filesystem::path root = SoundCatalog::FindProjectRoot());
    ~SoundManager();
    SoundManager(const SoundManager&) = delete;
    SoundManager& operator=(const SoundManager&) = delete;
    void Init();
    void Finalize();
    void Update();
    SoundHandle Play(std::string_view name);
    SoundHandle Preview(const SoundDefinition& definition);
    void Stop(SoundHandle handle);
    void Pause(SoundHandle handle);
    void Resume(SoundHandle handle);
    bool IsPlaying(SoundHandle handle) const;
    bool IsAlive(SoundHandle handle) const;
    void SetVolume(SoundHandle handle, float volume);
    void SetCategoryVolume(SoundCategory category, float volume);
    float GetCategoryVolume(SoundCategory category) const;
    void StopScope(SoundScope scope);
    void SetGamePaused(bool paused);
    void SetGameEnabled(bool enabled);
    SoundCatalog& GetCatalog() { return catalog_; }
    const std::string& GetLastError() const { return lastError_; }
private:
    struct Instance {
        std::shared_ptr<const SoundData> data;
        IXAudio2SourceVoice* voice = nullptr;
        SoundDefinition definition;
        SoundScope scope = SoundScope::Game;
        uint64_t generation = 0;
        float volume = 1.0f;
        bool paused = false;
        ~Instance() { if (voice) { voice->DestroyVoice(); } }
    };
    Instance* Find(SoundHandle handle);
    const Instance* Find(SoundHandle handle) const;
    SoundHandle Start(const SoundDefinition& definition, SoundScope scope);
    void ApplyVolume(Instance& instance);
    bool EffectivePause(const Instance& instance) const;
    void Fail(const std::string& message);
    Audio& audio_;
    SoundCatalog catalog_;
    std::vector<std::unique_ptr<Instance>> instances_;
    std::array<float, 3> categoryVolumes_{1, 1, 1};
    uint64_t nextGeneration_ = 1;
    bool gamePaused_ = false;
    bool gameEnabled_ = true;
    std::string lastError_;
};
}

#include "SoundManager.h"
#include "SoundDatabase.h"
#include "Engine/Utilities/Logger.h"
#include <algorithm>
#include <cmath>

namespace AOENGINE {
namespace {
float Gain(float value) { return std::isfinite(value) ? std::clamp(value, 0.0f, 1.0f) : 0.0f; }
}
SoundManager::SoundManager(Audio& audio, std::filesystem::path root) : audio_(audio), catalog_(std::move(root)) {}
SoundManager::~SoundManager() { Finalize(); }
void SoundManager::Fail(const std::string& message) {
    lastError_ = message;
    Logger::Log("[Sound] " + message + "\n");
}
void SoundManager::Init() {
    std::string error;
    catalog_.Load(error);
    if (!error.empty()) { Fail(error); }
}
void SoundManager::Finalize() { instances_.clear(); }
SoundHandle SoundManager::Play(std::string_view name) {
    if (!gameEnabled_) { return {}; }
    const auto* definition = catalog_.Find(name);
    if (!definition) { Fail("Unknown or duplicate sound name: " + std::string(name)); return {}; }
    return Start(*definition, SoundScope::Game);
}
SoundHandle SoundManager::Preview(const SoundDefinition& definition) {
    StopScope(SoundScope::Preview);
    return Start(definition, SoundScope::Preview);
}
SoundHandle SoundManager::Start(const SoundDefinition& definition, SoundScope scope) {
    lastError_.clear();
    const auto error = catalog_.Validate(definition);
    if (!error.empty()) { Fail(definition.name + ": " + error); return {}; }
    Update();
    size_t count = 0;
    size_t active = 0;
    for (const auto& instance : instances_) {
        if (instance) {
            ++active;
            if (instance->scope == scope && instance->definition.name == definition.name) { ++count; }
        }
    }
    if (count >= static_cast<size_t>(definition.maxInstances) || active >= 128) { return {}; }
    try {
        auto instance = std::make_unique<Instance>();
        instance->definition = definition;
        instance->scope = scope;
        instance->data = SoundDatabase::GetInstance()->GetSharedAudioData(catalog_.ResolveFile(definition.file), audio_);
        instance->voice = audio_.CreateSourceVoice(*instance->data);
        if (!instance->voice) { throw std::runtime_error("Cannot create source voice."); }
        XAUDIO2_BUFFER buffer{};
        buffer.pAudioData = instance->data->pBuffer.data();
        buffer.AudioBytes = instance->data->bufferSize;
        buffer.Flags = XAUDIO2_END_OF_STREAM;
        buffer.LoopCount = definition.loop ? XAUDIO2_LOOP_INFINITE : 0;
        if (FAILED(instance->voice->SubmitSourceBuffer(&buffer))) { throw std::runtime_error("Cannot submit audio buffer."); }
        ApplyVolume(*instance);
        if (!EffectivePause(*instance) && FAILED(instance->voice->Start())) { throw std::runtime_error("Cannot start voice."); }
        instance->generation = nextGeneration_++;
        size_t index = 0;
        while (index < instances_.size() && instances_[index]) { ++index; }
        SoundHandle handle{static_cast<uint32_t>(index), instance->generation};
        if (index == instances_.size()) { instances_.push_back(std::move(instance)); }
        else { instances_[index] = std::move(instance); }
        return handle;
    } catch (const std::exception& e) { Fail(definition.name + ": " + e.what()); return {}; }
}
const SoundManager::Instance* SoundManager::Find(SoundHandle handle) const {
    if (handle.index >= instances_.size()) { return nullptr; }
    const auto* instance = instances_[handle.index].get();
    return instance && instance->generation == handle.generation ? instance : nullptr;
}
SoundManager::Instance* SoundManager::Find(SoundHandle handle) {
    return const_cast<Instance*>(static_cast<const SoundManager*>(this)->Find(handle));
}
bool SoundManager::EffectivePause(const Instance& instance) const {
    return instance.paused || (instance.scope == SoundScope::Game && gamePaused_);
}
bool SoundManager::IsAlive(SoundHandle handle) const {
    const auto* instance = Find(handle);
    if (!instance) { return false; }
    XAUDIO2_VOICE_STATE state{};
    instance->voice->GetState(&state, XAUDIO2_VOICE_NOSAMPLESPLAYED);
    return state.BuffersQueued != 0;
}
bool SoundManager::IsPlaying(SoundHandle handle) const {
    const auto* instance = Find(handle);
    return instance && !EffectivePause(*instance) && IsAlive(handle);
}
void SoundManager::Stop(SoundHandle handle) { if (Find(handle)) { instances_[handle.index].reset(); } }
void SoundManager::Pause(SoundHandle handle) {
    if (auto* instance = Find(handle)) { instance->paused = true; instance->voice->Stop(); }
}
void SoundManager::Resume(SoundHandle handle) {
    if (auto* instance = Find(handle)) {
        instance->paused = false;
        if (!EffectivePause(*instance)) { instance->voice->Start(); }
    }
}
void SoundManager::ApplyVolume(Instance& instance) {
    instance.voice->SetVolume(instance.definition.volume * instance.volume *
        GetCategoryVolume(instance.definition.category) * Gain(Audio::GetMasterVolume()));
}
void SoundManager::SetVolume(SoundHandle handle, float volume) {
    if (auto* instance = Find(handle)) { instance->volume = Gain(volume); ApplyVolume(*instance); }
}
float SoundManager::GetCategoryVolume(SoundCategory category) const {
    const auto index = static_cast<size_t>(category);
    return index < categoryVolumes_.size() ? categoryVolumes_[index] : 0;
}
void SoundManager::SetCategoryVolume(SoundCategory category, float volume) {
    const auto index = static_cast<size_t>(category);
    if (index >= categoryVolumes_.size()) { return; }
    categoryVolumes_[index] = Gain(volume);
    for (auto& instance : instances_) { if (instance) { ApplyVolume(*instance); } }
}
void SoundManager::StopScope(SoundScope scope) {
    for (auto& instance : instances_) { if (instance && instance->scope == scope) { instance.reset(); } }
}
void SoundManager::SetGamePaused(bool paused) {
    gamePaused_ = paused;
    for (auto& instance : instances_) {
        if (!instance || instance->scope != SoundScope::Game) { continue; }
        if (EffectivePause(*instance)) { instance->voice->Stop(); }
        else { instance->voice->Start(); }
    }
}
void SoundManager::SetGameEnabled(bool enabled) {
    gameEnabled_ = enabled;
    if (!enabled) { StopScope(SoundScope::Game); }
    SetGamePaused(false);
}
void SoundManager::Update() {
    for (auto& instance : instances_) {
        if (!instance) { continue; }
        XAUDIO2_VOICE_STATE state{};
        instance->voice->GetState(&state, XAUDIO2_VOICE_NOSAMPLESPLAYED);
        if (state.BuffersQueued == 0) { instance.reset(); }
        else { ApplyVolume(*instance); }
    }
}
}

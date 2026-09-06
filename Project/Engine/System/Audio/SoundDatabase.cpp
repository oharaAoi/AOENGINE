#include "SoundDatabase.h"
#include "Engine/Core/Engine.h"
#include "Engine/Utilities/Logger.h"
#include <algorithm>
#include <cwctype>
#include <stdexcept>

using namespace AOENGINE;
SoundDatabase::~SoundDatabase() = default;
SoundDatabase* SoundDatabase::GetInstance() {
    static SoundDatabase instance;
    return &instance;
}
void SoundDatabase::Init() {
    audioLoadData_.clear();
    legacyNames_.clear();
}
std::shared_ptr<const SoundData> SoundDatabase::GetSharedAudioData(const std::filesystem::path& path, Audio& audio, bool forceReload) {
    auto canonical = std::filesystem::weakly_canonical(path);
    auto keyText = canonical.native();
    std::transform(keyText.begin(), keyText.end(), keyText.begin(), [](wchar_t c) { return static_cast<wchar_t>(std::towlower(c)); });
    const std::filesystem::path key(keyText);
    const auto modified = std::filesystem::last_write_time(canonical);
    const auto size = std::filesystem::file_size(canonical);
    auto it = audioLoadData_.find(key);
    if (!forceReload && it != audioLoadData_.end() && it->second.modified == modified && it->second.size == size) {
        return it->second.data;
    }
    // Old voices keep their PCM alive even when the cache entry is replaced.
    auto data = std::make_shared<const SoundData>(audio.LoadMP3(canonical.c_str()));
    audioLoadData_[key] = {data, modified, size};
    return data;
}
void SoundDatabase::AddMap(const std::string& directoryPath, const std::string& fileName, bool forceReload) {
    try {
        const auto path = std::filesystem::weakly_canonical(std::filesystem::path(directoryPath) / fileName);
        GetSharedAudioData(path, *Engine::GetAudio(), forceReload);
        auto [it, inserted] = legacyNames_.try_emplace(fileName, path);
        if (!inserted && it->second != path) {
            it->second.clear();
            Logger::Log("[Sound] Duplicate legacy filename: " + fileName + ". Use the sound table.\n");
        }
    } catch (const std::exception& e) { Logger::Log("[Sound] " + fileName + ": " + e.what() + "\n"); }
}
SoundData SoundDatabase::GetAudioData(const std::string& fileName) {
    auto it = legacyNames_.find(fileName);
    if (it == legacyNames_.end() || it->second.empty()) { throw std::runtime_error("Unknown or ambiguous audio filename: " + fileName); }
    return *GetSharedAudioData(it->second, *Engine::GetAudio());
}
void SoundDatabase::LoadAudio(const std::string& directoryPath, const std::string& fileName, bool forceReload) {
    GetInstance()->AddMap(directoryPath, fileName, forceReload);
}

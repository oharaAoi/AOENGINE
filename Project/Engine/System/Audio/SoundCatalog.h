#pragma once
#include <filesystem>
#include <string>
#include <string_view>
#include <vector>

namespace AOENGINE {
enum class SoundCategory { SE, BGM, UI };
const char* SoundCategoryName(SoundCategory category);

struct SoundDefinition {
    std::string name;
    std::string file;
    float volume = 1.0f;
    bool loop = false;
    SoundCategory category = SoundCategory::SE;
    int maxInstances = 4;
};

// Serialized paths are relative to the directory containing Assets.
class SoundCatalog {
public:
    explicit SoundCatalog(std::filesystem::path projectRoot = FindProjectRoot());
    static std::filesystem::path FindProjectRoot();
    const std::filesystem::path& GetProjectRoot() const { return root_; }
    std::filesystem::path GetTablePath() const;
    std::filesystem::path ResolveFile(const std::string& file) const;
    std::string MakeRelative(const std::filesystem::path& file) const;
    std::string Validate(const SoundDefinition& definition) const;
    std::vector<std::string> Validate(const std::vector<SoundDefinition>& rows) const;
    bool Load(std::string& error);
    bool Save(const std::vector<SoundDefinition>& rows, std::string& error);
    const SoundDefinition* Find(std::string_view name) const;
    const std::vector<SoundDefinition>& GetDefinitions() const { return definitions_; }
private:
    std::filesystem::path root_;
    std::vector<SoundDefinition> definitions_;
};
}

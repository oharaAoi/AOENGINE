#include "SoundCatalog.h"
#include <Windows.h>
#include <algorithm>
#include <cctype>
#include <cmath>
#include <fstream>
#include <unordered_map>
#include <nlohmann/json.hpp>

namespace AOENGINE {
namespace fs = std::filesystem;
const char* SoundCategoryName(SoundCategory category) {
    switch (category) {
    case SoundCategory::BGM: return "BGM";
    case SoundCategory::UI: return "UI";
    default: return "SE";
    }
}
SoundCatalog::SoundCatalog(fs::path projectRoot) : root_(fs::weakly_canonical(projectRoot)) {}
fs::path SoundCatalog::FindProjectRoot() {
    wchar_t executable[32768]{};
    GetModuleFileNameW(nullptr, executable, static_cast<DWORD>(std::size(executable)));
    for (auto start : {fs::current_path(), fs::path(executable).parent_path()}) {
        for (auto path = start; !path.empty(); path = path.parent_path()) {
            if (fs::is_directory(path / "Assets")) { return path; }
            if (fs::is_directory(path / "Project/Assets")) { return path / "Project"; }
            if (path == path.parent_path()) { break; }
        }
    }
    return fs::current_path();
}
fs::path SoundCatalog::GetTablePath() const { return root_ / "Assets/Game/Audio/SoundTable.json"; }
fs::path SoundCatalog::ResolveFile(const std::string& file) const { return fs::weakly_canonical(root_ / fs::u8path(file)); }
std::string SoundCatalog::MakeRelative(const fs::path& file) const {
    const auto value = fs::relative(fs::weakly_canonical(file), root_).generic_u8string();
    return std::string(value.begin(), value.end());
}
std::string SoundCatalog::Validate(const SoundDefinition& d) const {
    if (d.name.empty() || std::any_of(d.name.begin(), d.name.end(), [](unsigned char c) { return std::isspace(c) != 0; })) {
        return "Name is required and must not contain whitespace.";
    }
    if (!std::isfinite(d.volume) || d.volume < 0 || d.volume > 1) { return "Volume must be 0..1."; }
    if (d.maxInstances < 1 || d.maxInstances > 128) { return "Max instances must be 1..128."; }
    if (d.category != SoundCategory::SE && d.category != SoundCategory::BGM && d.category != SoundCategory::UI) { return "Invalid category."; }
    try {
        if (d.file.empty() || fs::u8path(d.file).is_absolute()) { return "Choose a project-relative audio file."; }
        auto path = ResolveFile(d.file);
        const auto relative = path.lexically_relative(root_);
        if (relative.empty() || *relative.begin() == "..") { return "File must be inside the project."; }
        auto ext = path.extension().string();
        std::transform(ext.begin(), ext.end(), ext.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        if (ext != ".wav" && ext != ".wave" && ext != ".mp3") { return "Choose a WAV or MP3 file."; }
        if (!fs::is_regular_file(path)) { return "Audio file does not exist."; }
    } catch (const std::exception& e) { return e.what(); }
    return {};
}
std::vector<std::string> SoundCatalog::Validate(const std::vector<SoundDefinition>& rows) const {
    std::unordered_map<std::string, size_t> counts;
    for (const auto& row : rows) { ++counts[row.name]; }
    std::vector<std::string> errors;
    for (const auto& row : rows) {
        errors.push_back(Validate(row));
        if (counts[row.name] > 1) { errors.back() = "Duplicate sound name."; }
    }
    return errors;
}
bool SoundCatalog::Load(std::string& error) {
    error.clear();
    try {
        std::ifstream stream(GetTablePath());
        if (!stream) { throw std::runtime_error("Cannot open sound table: " + GetTablePath().string()); }
        nlohmann::json json;
        stream >> json;
        if (json.at("version").get<int>() != 1 || !json.at("sounds").is_array()) { throw std::runtime_error("Unsupported sound table format."); }
        std::vector<SoundDefinition> rows;
        for (const auto& item : json.at("sounds")) {
            SoundDefinition d;
            d.name = item.at("name").get<std::string>();
            d.file = item.at("file").get<std::string>();
            d.volume = item.at("volume").get<float>();
            d.loop = item.at("loop").get<bool>();
            d.maxInstances = item.at("maxInstances").get<int>();
            const auto category = item.at("category").get<std::string>();
            if (category == "SE") { d.category = SoundCategory::SE; }
            else if (category == "BGM") { d.category = SoundCategory::BGM; }
            else if (category == "UI") { d.category = SoundCategory::UI; }
            else { throw std::runtime_error("Unknown sound category: " + category); }
            rows.push_back(std::move(d));
        }
        // Keep invalid rows editable (e.g. a missing file); playback validates each request.
        definitions_ = std::move(rows);
        const auto errors = Validate(definitions_);
        for (size_t i = 0; i < errors.size(); ++i) {
            if (!errors[i].empty()) { error += definitions_[i].name + ": " + errors[i] + "\n"; }
        }
        return true;
    } catch (const std::exception& e) { error = e.what(); return false; }
}
bool SoundCatalog::Save(const std::vector<SoundDefinition>& rows, std::string& error) {
    error.clear();
    for (const auto& message : Validate(rows)) {
        if (!message.empty()) { error = message; return false; }
    }
    try {
        auto normalized = rows;
        nlohmann::json json{{"version", 1}, {"sounds", nlohmann::json::array()}};
        for (auto& d : normalized) {
            d.file = MakeRelative(ResolveFile(d.file));
            json["sounds"].push_back({{"name", d.name}, {"file", d.file}, {"volume", d.volume},
                {"loop", d.loop}, {"category", SoundCategoryName(d.category)}, {"maxInstances", d.maxInstances}});
        }
        fs::create_directories(GetTablePath().parent_path());
        auto temporary = GetTablePath(); temporary += ".tmp";
        std::ofstream stream(temporary, std::ios::trunc);
        stream << json.dump(2) << '\n';
        stream.close();
        if (!stream) { throw std::runtime_error("Could not write sound table."); }
        if (!MoveFileExW(temporary.c_str(), GetTablePath().c_str(), MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH)) {
            throw std::runtime_error("Could not replace sound table. Previous table was preserved.");
        }
        definitions_ = std::move(normalized);
        return true;
    } catch (const std::exception& e) { error = e.what(); return false; }
}
const SoundDefinition* SoundCatalog::Find(std::string_view name) const {
    const SoundDefinition* result = nullptr;
    for (const auto& d : definitions_) {
        if (d.name == name) {
            if (result) { return nullptr; } // Ambiguous names must never select an arbitrary row.
            result = &d;
        }
    }
    return result;
}
}

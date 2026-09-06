#include "SoundTableWindow.h"
#include "Engine/Core/Engine.h"
#include "Engine/System/Manager/ImGuiManager.h"
#include <algorithm>
#include <cctype>

namespace AOENGINE {
namespace {
bool EditString(const char* label, std::string& value) {
    std::vector<char> buffer(value.begin(), value.end());
    buffer.resize((std::max)(size_t{4096}, value.size() + 1024), '\0');
    if (!ImGui::InputText(label, buffer.data(), buffer.size())) { return false; }
    value = buffer.data();
    return true;
}
std::string Lower(std::string text) {
    std::transform(text.begin(), text.end(), text.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return text;
}
}
void SoundTableWindow::Init() {
    name_ = "Sound Table";
    rows_ = Engine::GetSoundManager()->GetCatalog().GetDefinitions();
    message_ = Engine::GetSoundManager()->GetLastError();
}
void SoundTableWindow::Load() {
    auto& catalog = Engine::GetSoundManager()->GetCatalog();
    if (catalog.Load(message_)) { rows_ = catalog.GetDefinitions(); dirty_ = false; }
}
std::string SoundTableWindow::UniqueName(std::string name) const {
    const auto base = name;
    int suffix = 1;
    while (std::any_of(rows_.begin(), rows_.end(), [&](const auto& row) { return row.name == name; })) {
        name = base + std::to_string(suffix++);
    }
    return name;
}
void SoundTableWindow::SelectFile(size_t row, const std::filesystem::path& path) {
    try {
        auto& catalog = Engine::GetSoundManager()->GetCatalog();
        auto candidate = rows_.at(row);
        candidate.file = catalog.MakeRelative(path);
        // Validate the file without requiring the draft's other fields to be complete.
        SoundDefinition fileCheck;
        fileCheck.name = "SelectedAudio";
        fileCheck.file = candidate.file;
        if (const auto error = catalog.Validate(fileCheck); !error.empty()) {
            message_ = error;
            return;
        }
        rows_[row].file = std::move(candidate.file);
        message_.clear();
        dirty_ = true;
    } catch (const std::exception& e) { message_ = e.what(); }
}
void SoundTableWindow::UpdateVisibility() {
    if (!isActive_) {
        Engine::GetSoundManager()->Stop(preview_);
        preview_ = {};
        if (dialogRow_ >= 0) { ImGuiFileDialog::Instance()->Close(); dialogRow_ = -1; }
    }
}
void SoundTableWindow::Edit() {
    auto& manager = *Engine::GetSoundManager();
    auto& catalog = manager.GetCatalog();
    ImGui::SetNextWindowSize(ImVec2(1100, 480), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin(name_.c_str(), &isActive_)) { ImGui::End(); UpdateVisibility(); return; }
    ImGui::TextUnformatted("Call Play(\"Name\") from code. Renaming a sound also requires updating code references.");
    ImGui::TextUnformatted("Save applies settings to future playback. Preview uses this draft. WAV / MP3, volume 0..1.");
    ImGui::TextDisabled("%s%s", catalog.GetTablePath().string().c_str(), dirty_ ? "  * Unsaved" : "");
    ImGui::BeginDisabled(dialogRow_ >= 0);
    if (ImGui::Button("Add")) {
        SoundDefinition row; row.name = UniqueName("Sound"); rows_.push_back(std::move(row)); dirty_ = true;
    }
    ImGui::SameLine();
    if (ImGui::Button("Save")) {
        if (catalog.Save(rows_, message_)) { rows_ = catalog.GetDefinitions(); dirty_ = false; message_ = "Saved."; }
    }
    ImGui::SameLine();
    if (ImGui::Button("Reload")) {
        if (dirty_) { ImGui::OpenPopup("Discard sound edits?"); } else { Load(); }
    }
    if (ImGui::BeginPopupModal("Discard sound edits?", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::TextUnformatted("Reload the saved table and discard unsaved changes?");
        if (ImGui::Button("Discard and reload")) { Load(); ImGui::CloseCurrentPopup(); }
        ImGui::SameLine();
        if (ImGui::Button("Cancel")) { ImGui::CloseCurrentPopup(); }
        ImGui::EndPopup();
    }
    ImGui::SameLine();
    if (ImGui::Button("Stop preview")) { manager.Stop(preview_); preview_ = {}; }
    ImGui::SameLine();
    ImGui::TextUnformatted(manager.IsPlaying(preview_) ? "Preview playing" : "");
    EditString("Search name / file", search_);
    if (!message_.empty()) { ImGui::TextWrapped("%s", message_.c_str()); }
    float master = Audio::GetMasterVolume();
    ImGui::SetNextItemWidth(140);
    if (ImGui::SliderFloat("Master", &master, 0, 1)) { Audio::SetMasterVolume(master); }
    for (auto category : {SoundCategory::SE, SoundCategory::BGM, SoundCategory::UI}) {
        ImGui::SameLine(); ImGui::SetNextItemWidth(110);
        float volume = manager.GetCategoryVolume(category);
        if (ImGui::SliderFloat(SoundCategoryName(category), &volume, 0, 1)) { manager.SetCategoryVolume(category, volume); }
    }
    ImGui::TextDisabled("Master / category sliders are session settings.");
    int remove = -1, duplicate = -1;
    if (ImGui::BeginTable("Sounds", 8, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollX)) {
        for (const char* title : {"Name", "File", "Volume", "Loop", "Category", "Max", "Actions", "Status"}) { ImGui::TableSetupColumn(title); }
        ImGui::TableHeadersRow();
        const auto query = Lower(search_);
        for (size_t i = 0; i < rows_.size(); ++i) {
            auto& row = rows_[i];
            if (!query.empty() && Lower(row.name + " " + row.file).find(query) == std::string::npos) { continue; }
            ImGui::PushID(static_cast<int>(i));
            ImGui::TableNextRow(); ImGui::TableNextColumn();
            ImGui::SetNextItemWidth(-1); dirty_ |= EditString("##name", row.name);
            ImGui::TableNextColumn();
            ImGui::BeginGroup();
            ImGui::SetNextItemWidth(-1); dirty_ |= EditString("##file", row.file);
            if (ImGui::SmallButton("Browse...")) {
                dialogRow_ = static_cast<int>(i);
                IGFD::FileDialogConfig config; config.path = (catalog.GetProjectRoot() / "Assets").string();
                ImGuiFileDialog::Instance()->OpenDialog("SoundFile", "Select sound", ".wav,.wave,.mp3,.WAV,.WAVE,.MP3", config);
            }
            ImGui::TextDisabled("Drop .mp3 / .wav / .wave here");
            ImGui::EndGroup();
            // The whole File cell (input, Browse button and hint) accepts the asset.
            if (dialogRow_ < 0 && ImGui::BeginDragDropTarget()) {
                if (const auto* payload = ImGui::AcceptDragDropPayload("SOUND_ASSET_PATH")) {
                    if (payload->DataSize > 1) {
                        const auto* bytes = static_cast<const char*>(payload->Data);
                        if (bytes[payload->DataSize - 1] == '\0') { SelectFile(i, std::filesystem::u8path(bytes)); }
                    }
                }
                ImGui::EndDragDropTarget();
            }
            ImGui::TableNextColumn(); ImGui::SetNextItemWidth(-1);
            dirty_ |= ImGui::DragFloat("##volume", &row.volume, 0.01f, 0, 1);
            ImGui::TableNextColumn(); dirty_ |= ImGui::Checkbox("##loop", &row.loop);
            ImGui::TableNextColumn(); ImGui::SetNextItemWidth(-1);
            int category = static_cast<int>(row.category);
            if (ImGui::Combo("##category", &category, "SE\0BGM\0UI\0")) { row.category = static_cast<SoundCategory>(category); dirty_ = true; }
            ImGui::TableNextColumn(); ImGui::SetNextItemWidth(-1);
            dirty_ |= ImGui::InputInt("##max", &row.maxInstances);
            ImGui::TableNextColumn();
            if (ImGui::SmallButton("Preview")) { preview_ = manager.Preview(row); message_ = manager.GetLastError(); }
            ImGui::SameLine();
            if (ImGui::SmallButton("Copy")) { duplicate = static_cast<int>(i); }
            ImGui::SameLine();
            if (ImGui::SmallButton("Delete")) { remove = static_cast<int>(i); }
            ImGui::TableNextColumn();
            auto error = catalog.Validate(row);
            if (std::count_if(rows_.begin(), rows_.end(), [&](const auto& other) { return other.name == row.name; }) > 1) { error = "Duplicate name."; }
            if (!error.empty()) { ImGui::TextColored(ImVec4(1, 0.4f, 0.3f, 1), "%s", error.c_str()); }
            ImGui::PopID();
        }
        ImGui::EndTable();
    }
    if (duplicate >= 0) {
        auto row = rows_[duplicate]; row.name = UniqueName(row.name + "Copy"); rows_.push_back(std::move(row)); dirty_ = true;
    }
    if (remove >= 0) { rows_.erase(rows_.begin() + remove); dirty_ = true; }
    ImGui::EndDisabled();
    if (dialogRow_ >= 0 && ImGuiFileDialog::Instance()->Display("SoundFile")) {
        if (ImGuiFileDialog::Instance()->IsOk()) { SelectFile(static_cast<size_t>(dialogRow_), std::filesystem::u8path(ImGuiFileDialog::Instance()->GetFilePathName())); }
        ImGuiFileDialog::Instance()->Close(); dialogRow_ = -1;
    }
    ImGui::End();
    UpdateVisibility();
}
}

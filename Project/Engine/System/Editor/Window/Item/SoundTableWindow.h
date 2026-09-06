#pragma once
#include "IWindowItem.h"
#include "Engine/System/Audio/SoundManager.h"

namespace AOENGINE {
class SoundTableWindow final : public IWindowItem {
public:
    void Init() override;
    void Edit() override;
    // Called even when hidden through the Window menu.
    void UpdateVisibility();
private:
    void Load();
    std::string UniqueName(std::string name) const;
    void SelectFile(size_t row, const std::filesystem::path& path);
    std::vector<SoundDefinition> rows_;
    std::string search_;
    std::string message_;
    SoundHandle preview_;
    int dialogRow_ = -1;
    bool dirty_ = false;
};
}

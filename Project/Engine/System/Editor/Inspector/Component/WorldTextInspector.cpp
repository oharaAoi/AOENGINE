#include "WorldTextInspector.h"

#include <algorithm>
#include <cstring>
#include <filesystem>
#include <vector>
#include "Engine/Module/Components/3d/WorldTextComponent.h"
#include "Engine/Module/Components/GameObject/BaseGameObject.h"
#include "Engine/System/Manager/FontManager.h"
#include "Engine/System/Manager/ImGuiManager.h"

using namespace AOENGINE;

void WorldTextInspector::Draw(BaseGameObject& object) {
	WorldTextComponent* component = object.GetWorldTextComponent();
	if (!component) { return; }

	bool remove = false;
	const bool open = ImGui::CollapsingHeader("World Text");
	if (ImGui::BeginPopupContextItem("WorldTextContext")) {
		remove = ImGui::MenuItem("Remove Component");
		ImGui::EndPopup();
	}

	if (open) {
		const WorldTextSettings& current = component->GetSettings();
		bool enabled = current.enabled;
		if (ImGui::Checkbox("Enabled##WorldText", &enabled)) { component->SetEnabled(enabled); }

		constexpr size_t kMaxTextLength = 4096;
		std::vector<char> buffer(kMaxTextLength, '\0');
		std::memcpy(buffer.data(), current.text.c_str(), (std::min)(current.text.size(), kMaxTextLength - 1));
		ImGui::TextUnformatted("Text");
		if (ImGui::InputTextMultiline("##worldText", buffer.data(), buffer.size(), ImVec2(-1.0f, 90.0f))) {
			component->SetText(buffer.data());
		}

		const auto& fontPaths = FontManager::GetInstance()->GetFontPaths();
		std::string selectedFont = std::filesystem::path(current.fontPath).filename().string();
		if (selectedFont.empty()) { selectedFont = "Default"; }
		if (ImGui::BeginCombo("Font##WorldText", selectedFont.c_str())) {
			for (const std::string& path : fontPaths) {
				const std::string name = std::filesystem::path(path).filename().string();
				if (ImGui::Selectable(name.c_str(), path == current.fontPath)) { component->SetFontPath(path); }
			}
			ImGui::EndCombo();
		}

		float fontSize = current.fontSize;
		if (ImGui::DragFloat("Font Size", &fontSize, 1.0f, 1.0f, 512.0f)) { component->SetFontSize(fontSize); }
		Color color = current.color;
		if (ImGui::ColorEdit4("Color##WorldText", &color.r)) { component->SetColor(color); }
		Math::Vector3 position = current.localPosition;
		if (ImGui::DragFloat3("Local Position", &position.x, 0.01f)) { component->SetLocalPosition(position); }
		Math::Quaternion rotation = current.localRotation;
		if (ImGui::DragFloat4("Local Rotation", &rotation.x, 0.01f)) { component->SetLocalRotation(rotation.Normalize()); }
		float height = current.height;
		if (ImGui::DragFloat("Height", &height, 0.01f, 0.001f, 100.0f)) { component->SetHeight(height); }

		const char* modes[] = { "None", "Face Camera", "Y Axis Only" };
		int mode = static_cast<int>(current.billboardMode);
		if (ImGui::Combo("Billboard", &mode, modes, IM_ARRAYSIZE(modes))) {
			component->SetBillboardMode(static_cast<WorldTextBillboardMode>(mode));
		}
		bool depthTest = current.depthTest;
		if (ImGui::Checkbox("Depth Test##WorldText", &depthTest)) { component->SetDepthTest(depthTest); }
	}

	if (remove) { object.RemoveWorldTextComponent(); }
}

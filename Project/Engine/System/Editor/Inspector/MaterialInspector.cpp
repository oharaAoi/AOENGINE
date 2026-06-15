#include "MaterialInspector.h"
#include "Engine/System/Manager/ImGuiManager.h"
#include <string>

void AOENGINE::MaterialInspector::Draw(BaseGameObject& object) {
	if (!ImGui::CollapsingHeader("Material")) {
		return;
	}

	const auto& materials = object.GetMaterials();
	if (materials.empty()) {
		ImGui::TextUnformatted("No materials");
		return;
	}

	int index = 0;
	for (const auto& material : materials) {
		const std::string& materialName = material.first;
		if (!materialName.empty()) {
			ImGui::PushID(materialName.c_str());
		} else {
			ImGui::PushID(index);
		}

		const char* label = materialName.empty() ? "Material" : materialName.c_str();
		if (ImGui::TreeNode(label)) {
			if (material.second) {
				material.second->Debug_Gui();
			} else {
				ImGui::TextUnformatted("null");
			}
			ImGui::TreePop();
		}

		ImGui::PopID();
		++index;
	}
}

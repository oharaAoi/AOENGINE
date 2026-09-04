#include "MaterialInspector.h"
#include "Engine/System/Manager/ImGuiManager.h"
#include "Engine/System/Manager/ShaderGraphManager.h"
#include <string>

//////////////////////////////////////////////////////////////////////////////////////////////////
// Material一覧のInspectorを描画する
//////////////////////////////////////////////////////////////////////////////////////////////////

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
				ImGui::Button("ShaderGraphをここへドロップ", ImVec2(-1.0f, 0.0f));
				if (ImGui::BeginDragDropTarget()) {
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_HANDLE")) {
						if (payload->DataSize == sizeof(AssetHandle)) {
							const AssetHandle handle = *static_cast<const AssetHandle*>(payload->Data);
							if (handle.type == AssetType::ShaderGraph) {
								ShaderGraphManager* manager = ShaderGraphManager::GetInstance();
								material.second->SetShaderGraph(manager->Resolve(handle), manager->GetPath(handle));
							}
						}
					}
					ImGui::EndDragDropTarget();
				}
				if (!material.second->GetShaderGraphAssetPath().empty()) {
					ImGui::TextWrapped("Graph: %s", material.second->GetShaderGraphAssetPath().c_str());
				}
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

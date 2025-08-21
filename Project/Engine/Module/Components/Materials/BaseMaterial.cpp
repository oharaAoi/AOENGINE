#include "BaseMaterial.h"
#include "imgui.h"

void BaseMaterial::Debug_UV() {
	if (ImGui::TreeNode("uvTramsform")) {
		if (ImGui::TreeNode("scale")) {
			ImGui::DragFloat4("uvScale", &uvTransform_.scale.x, 0.01f);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("rotation")) {
			ImGui::DragFloat4("uvRotation", &uvTransform_.rotate.x, 0.01f);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("translation")) {
			ImGui::DragFloat4("uvTranslation", &uvTransform_.translate.x, 0.01f);
			ImGui::TreePop();
		}
		ImGui::TreePop();
	}
}

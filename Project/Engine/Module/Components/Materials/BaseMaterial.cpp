#include "BaseMaterial.h"
#include "Engine/System/Manager/ImGuiManager.h"
#include "Engine/Utilities/ImGuiHelperFunc.h"
#include <vector>

void BaseMaterial::EditUV() {
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

void BaseMaterial::EditShaderType() {
	std::vector<std::string> typeArray = { "Universal Render", "ShaderGraph Render" };
	shaderTypeIndex_ = ContainerOfComb(typeArray, shaderTypeIndex_, "ShaderType##Material");
	shaderType_ = static_cast<MaterialShaderType>(shaderTypeIndex_);
}

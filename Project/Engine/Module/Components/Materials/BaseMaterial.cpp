#include "BaseMaterial.h"
#include "Engine/System/Manager/ImGuiManager.h"
#include "Engine/Utilities/ImGuiHelperFunc.h"
#include "Engine/System/Manager/ShaderGraphManager.h"
#include <vector>

using namespace AOENGINE;

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

	if (shaderType_ == MaterialShaderType::ShaderGraphRender) {
		if (shaderGraph_) {
			shaderGraph_->Debug_Gui();
		}
	}
}

void BaseMaterial::SetShaderGraph(ShaderGraph* _graph) {
	if (_graph) {
		shaderGraphAsset_.reset();
		shaderGraphAssetPath_.clear();
		shaderGraph_ = _graph;
		shaderType_ = MaterialShaderType::ShaderGraphRender;
		shaderTypeIndex_ = static_cast<int>(shaderType_);
	}
}

void BaseMaterial::UpdateShaderGraph() {
	if (shaderType_ == MaterialShaderType::ShaderGraphRender && shaderGraph_) {
		shaderGraph_->Update();
	}
}

void BaseMaterial::SetShaderGraph(std::shared_ptr<ShaderGraph> _graph, const std::string& _assetPath) {
	shaderGraphAsset_ = std::move(_graph);
	shaderGraph_ = shaderGraphAsset_.get();
	shaderGraphAssetPath_ = _assetPath;
	if (shaderGraph_) {
		shaderType_ = MaterialShaderType::ShaderGraphRender;
		shaderTypeIndex_ = static_cast<int>(shaderType_);
	}
}

bool BaseMaterial::SetShaderGraphAsset(const std::string& _assetPath) {
	auto graph = ShaderGraphManager::GetInstance()->Load(_assetPath);
	if (!graph) { return false; }
	SetShaderGraph(std::move(graph), _assetPath);
	return true;
}

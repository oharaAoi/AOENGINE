#include "BaseMaterial.h"
#include "Engine/System/Manager/ImGuiManager.h"
#include "Engine/Utilities/ImGuiHelperFunc.h"
#include "Engine/System/Manager/ShaderGraphManager.h"
#include <vector>
#include <algorithm>
#include "Engine/Core/GraphicsContext.h"
#include "PBRMaterial.h"
#include "MaterialPipelinePolicy.h"

using namespace AOENGINE;

bool BaseMaterial::IsPipelineCompatible(const Pipeline& pipeline) const {
	const auto& parameters = pipeline.GetParameters();
	return IsMaterialPipelineCompatible(dynamic_cast<const PBRMaterial*>(this) != nullptr,
		parameters.vs, parameters.ps, parameters.rtvFormats, parameters.primitiveTopologyType);
}

const Pipeline* BaseMaterial::ResolvePipeline(const Pipeline* fallback) const {
	if (pipelineName_.empty()) { return fallback; }
	const auto* pipelines = GraphicsContext::GetInstance()->GetGraphicsPipeline();
	const auto* pipeline = pipelines ? pipelines->FindObjectPipeline(pipelineName_) : nullptr;
	return pipeline && IsPipelineCompatible(*pipeline) ? pipeline : fallback;
}

void BaseMaterial::EditPipeline() {
	const auto* pipelines = GraphicsContext::GetInstance()->GetGraphicsPipeline();
	if (!pipelines) { return; }
	const char* preview = pipelineName_.empty() ? "Use Object Pipeline" : pipelineName_.c_str();
	if (ImGui::BeginCombo("Pipeline", preview)) {
		if (ImGui::Selectable("Use Object Pipeline", pipelineName_.empty())) { pipelineName_.clear(); }
		auto names = pipelines->GetObjectPipelineNames();
		std::sort(names.begin(), names.end());
		for (const auto& name : names) {
			const auto* pipeline = pipelines->FindObjectPipeline(name);
			if (!pipeline || !IsPipelineCompatible(*pipeline)) { continue; }
			const bool selected = name == pipelineName_;
			if (ImGui::Selectable(name.c_str(), selected)) { pipelineName_ = name; }
			if (selected) { ImGui::SetItemDefaultFocus(); }
		}
		ImGui::EndCombo();
	}
	if (!pipelineName_.empty() && !ResolvePipeline(nullptr)) {
		ImGui::TextWrapped("Pipeline unavailable or incompatible. Using the object pipeline.");
	}
}

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

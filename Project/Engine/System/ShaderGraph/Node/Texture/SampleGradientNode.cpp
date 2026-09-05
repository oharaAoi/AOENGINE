#include "SampleGradientNode.h"
#include "Engine/Core/Engine.h"
#include "Engine/System/ShaderGraph/ShaderGraphHelperFunc.h"
#include "Engine/Lib/CalcDispatchSize2D.h"

using namespace AOENGINE;

SampleGradientNode::~SampleGradientNode() {
	if (outputResource_) { outputResource_->Destroy(); }
}

void SampleGradientNode::Init() {
	ctx_ = GraphicsContext::GetInstance();
	addIN<DxResource*>("Texture", nullptr, ImFlow::ConnectionFilter::SameType());
	addIN<GradientData>("Gradient", GradientData{}, ImFlow::ConnectionFilter::SameType());
	addOUT<DxResource*>("DxResource", ImFlow::PinStyle::green())->behaviour([this]() {
		return hasOutput_ ? outputResource_ : nullptr;
	});
	buffer_ = CreateBufferResource(ctx_->GetDevice(), sizeof(Params));
	const HRESULT hr = buffer_->Map(0, nullptr, reinterpret_cast<void**>(&param_));
	assert(SUCCEEDED(hr));
	*param_ = {};
	SetTitleBar(ImColor(46, 139, 87));
}

void SampleGradientNode::customUpdate() {
	hasOutput_ = false;
	auto* input = getInVal<DxResource*>("Texture");
	if (!input || !input->GetResource()) { return; }
	auto gradient = getInVal<GradientData>("Gradient");
	gradient.Normalize();
	const auto* desc = input->GetDesc();
	if (!outputResource_ || outputResource_->GetDesc()->Width != desc->Width || outputResource_->GetDesc()->Height != desc->Height) {
		if (outputResource_) { outputResource_->Destroy(); }
		outputResource_ = ctx_->CreateDxResource(ResourceType::Common);
		CreteOutputResource(outputResource_, input, static_cast<UINT>(desc->Width), desc->Height);
	}
	*param_ = {};
	param_->keyCount = static_cast<uint32_t>(gradient.keys.size());
	param_->channel = static_cast<uint32_t>(channel_);
	param_->multiplySourceAlpha = multiplySourceAlpha_ ? 1u : 0u;
	for (size_t i = 0; i < gradient.keys.size(); ++i) {
		param_->keys[i].color = gradient.keys[i].color;
		param_->keys[i].position = gradient.keys[i].position;
	}

	auto* commandList = ctx_->GetCommandList();
	const auto inputState = input->GetState();
	input->Transition(commandList, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
	outputResource_->Transition(commandList, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	Pipeline* pso = Engine::SetPipelineCS("SampleGradient.json");
	commandList->SetComputeRootDescriptorTable(pso->GetRootSignatureIndex("gInputTex"), input->GetSRV().handleGPU);
	commandList->SetComputeRootDescriptorTable(pso->GetRootSignatureIndex("gOutputTex"), outputResource_->GetUAV().handleGPU);
	commandList->SetComputeRootConstantBufferView(pso->GetRootSignatureIndex("gGradient"), buffer_->GetGPUVirtualAddress());
	const auto group = CalcDispatchSize(*outputResource_->GetDesc());
	commandList->Dispatch(group.x, group.y, 1);
	outputResource_->Transition(commandList, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
	input->Transition(commandList, inputState);
	hasOutput_ = true;
}

void SampleGradientNode::updateGui() {
	ImGui::PushID(this);
	ImGui::Combo("Channel", &channel_, "R\0G\0B\0A\0Luminance\0");
	ImGui::Checkbox("Multiply Source Alpha", &multiplySourceAlpha_);
	ImGui::PopID();
}

void SampleGradientNode::draw() {
	if (hasOutput_ && outputResource_ && outputResource_->GetResource()) {
		const ImTextureID texture = reinterpret_cast<ImTextureID>(outputResource_->GetSRV().handleGPU.ptr);
		ImGui::Image(texture, ImVec2(64, 64));
	}
}

nlohmann::json SampleGradientNode::toJson() {
	nlohmann::json result;
	BaseInfoToJson(result);
	result["props"]["channel"] = channel_;
	result["props"]["multiplySourceAlpha"] = multiplySourceAlpha_;
	return result;
}

void SampleGradientNode::fromJson(const nlohmann::json& json) {
	BaseInfoFromJson(json);
	if (json.contains("props")) {
		channel_ = std::clamp(json["props"].value("channel", 0), 0, 4);
		multiplySourceAlpha_ = json["props"].value("multiplySourceAlpha", true);
	}
}

#include "DistortionNode.h"
#include "Engine/Core/Engine.h"
#include "Engine/Lib/GameTimer.h"
#include "Engine/System/ShaderGraph/ShaderGraphHelperFunc.h"
#include "Engine/Lib/CalcDispatchSize2D.h"

using namespace AOENGINE;

DistortionNode::~DistortionNode() {
	outputResource_->Destroy();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　初期化関数
//////////////////////////////////////////////////////////////////////////////////////////////////

void DistortionNode::Init() {
	ctx_ = AOENGINE::GraphicsContext::GetInstance();
	cmdList_ = ctx_->GetCommandList();

	// inputの設定
	addIN<AOENGINE::DxResource*>("BaseTexture", nullptr, ImFlow::ConnectionFilter::None());
	addIN<AOENGINE::DxResource*>("SampleNoise", nullptr, ImFlow::ConnectionFilter::None());

	// バッファを確保
	buffer_ = CreateBufferResource(ctx_->GetDevice(), sizeof(DistortionParam));
	buffer_->Map(0, nullptr, reinterpret_cast<void**>(&param_));

	// outputの設定
	outputResource_ = ctx_->CreateDxResource(ResourceType::Common);
	auto texOut = addOUT<AOENGINE::DxResource*>("DxResource", ImFlow::PinStyle::green());
	texOut->behaviour([this]() { return outputResource_; });

	// titleBarのカラーを設定
	SetTitleBar(ImColor(255, 99, 71));

	// parameterの初期化
	param_->scroll = CMath::Vector2::UNIT;
	param_->tiling = CMath::Vector2::UNIT;
	param_->strengthX = 0.1f;
	param_->strengthY = 0.1f;
	param_->time = 0.f;
}

void DistortionNode::customUpdate() {
	inputBaseResource_ = getInVal<AOENGINE::DxResource*>("BaseTexture");
	inputNoiseResource_ = getInVal<AOENGINE::DxResource*>("SampleNoise");

	param_->time += AOENGINE::GameTimer::DeltaTime();

	// resourceの作成
	CreateView();

	// csの実行
	ExecuteCommand();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　Guiの更新
//////////////////////////////////////////////////////////////////////////////////////////////////

void DistortionNode::updateGui() {
	ImGui::DragFloat2("tiling", &param_->tiling.x, 1);
	ImGui::DragFloat2("scroll", &param_->scroll.x, 1);
	ImGui::DragFloat("strengthX", &param_->strengthX, 0.1f);
	ImGui::DragFloat("strengthY", &param_->strengthY, 0.1f);
	ImGui::DragFloat("time", &param_->time);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　nodeの描画
//////////////////////////////////////////////////////////////////////////////////////////////////

void DistortionNode::draw() {
	// -------- 内部プレビュー ----------
	if (outputResource_->GetResource()) {
		if (inputBaseResource_) {
			ImTextureID texID = (ImTextureID)(intptr_t)(outputResource_->GetSRV().handleGPU.ptr);
			ImGui::SetNextWindowBgAlpha(0.85f);
			ImGui::Image(texID, ImVec2(64, 64));
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　json形式に変換
//////////////////////////////////////////////////////////////////////////////////////////////////

nlohmann::json DistortionNode::toJson() {
	nlohmann::json result;
	BaseInfoToJson(result);
	result["props"]["tiling"] = Convert::toJson<Math::Vector2>(param_->tiling);
	result["props"]["scroll"] = Convert::toJson<Math::Vector2>(param_->scroll);
	result["props"]["strengthX"] = Convert::toJson<float>(param_->strengthX);
	result["props"]["strengthY"] = Convert::toJson<float>(param_->strengthY);
	return result;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　json形式から変換
//////////////////////////////////////////////////////////////////////////////////////////////////

void DistortionNode::fromJson(const nlohmann::json& _json) {
	BaseInfoFromJson(_json);

	if (_json.contains("props")) {
		const auto& props = _json.value("props", nlohmann::json::object());

		const auto& scroll = props.value("scroll", nlohmann::json::object());
		param_->scroll.x = scroll.value("x", 0.0f);
		param_->scroll.y = scroll.value("y", 0.0f);

		param_->strengthX = props.value("strengthX", 0.0f);
		param_->strengthY = props.value("strengthY", 0.0f);

		const auto& tiling = props.value("tiling", nlohmann::json::object());
		param_->tiling.x = tiling.value("x", 1.0f);
		param_->tiling.y = tiling.value("y", 1.0f);
		
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　viewの作成
//////////////////////////////////////////////////////////////////////////////////////////////////

void DistortionNode::CreateView() {
	if (inputBaseResource_) {
		if (!outputResource_->GetResource()) {
			CreteOutputResource(outputResource_, inputBaseResource_);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　commandの実行
//////////////////////////////////////////////////////////////////////////////////////////////////

void DistortionNode::ExecuteCommand() {
	if (inputBaseResource_ && inputNoiseResource_) {
		if (outputResource_->GetResource()) {
			outputResource_->Transition(cmdList_, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

			Pipeline* pso = Engine::SetPipelineCS("Distortion.json");
			UINT index = 0;
			index = pso->GetRootSignatureIndex("gBaseTex");
			cmdList_->SetComputeRootDescriptorTable(index, inputBaseResource_->GetSRV().handleGPU);
			index = pso->GetRootSignatureIndex("gNoiseTex");
			cmdList_->SetComputeRootDescriptorTable(index, inputNoiseResource_->GetSRV().handleGPU);
			index = pso->GetRootSignatureIndex("outputTex");
			cmdList_->SetComputeRootDescriptorTable(index, outputResource_->GetUAV().handleGPU);
			index = pso->GetRootSignatureIndex("gDistortion");
			cmdList_->SetComputeRootConstantBufferView(index, buffer_->GetGPUVirtualAddress());

			auto desc = outputResource_->GetDesc();
			ComputeDispatchSize2D group = CalcDispatchSize(*desc);
			cmdList_->Dispatch(group.x, group.y, 1);

			outputResource_->Transition(cmdList_, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
		}
	}
}

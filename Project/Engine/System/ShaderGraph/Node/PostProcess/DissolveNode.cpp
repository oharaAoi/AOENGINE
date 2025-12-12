#include "DissolveNode.h"
#include "Engine/Engine.h"
#include "Engine/System/ShaderGraph/ShaderGraphHelperFunc.h"

using namespace AOENGINE;

DissolveNode::~DissolveNode() {
	outputResource_->Destroy();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　初期化関数
//////////////////////////////////////////////////////////////////////////////////////////////////

void DissolveNode::Init() {
	ctx_ = AOENGINE::GraphicsContext::GetInstance();
	cmdList_ = ctx_->GetCommandList();

	// inputの設定
	addIN<AOENGINE::DxResource*>("BaseTex", nullptr, ImFlow::ConnectionFilter::None());
	addIN<AOENGINE::DxResource*>("NoiseTex", nullptr, ImFlow::ConnectionFilter::None());
	addIN<float>("threshold", threshold_, ImFlow::ConnectionFilter::None());

	// paremterのバッファを確保
	buffer_ = CreateBufferResource(ctx_->GetDevice(), sizeof(DissolveParams));
	buffer_->Map(0, nullptr, reinterpret_cast<void**>(&param_));

	outputResource_ = ctx_->CreateDxResource(ResourceType::Common);
	auto texOut = addOUT<AOENGINE::DxResource*>("DxResource", ImFlow::PinStyle::green());
	texOut->behaviour([this]() { return outputResource_; });

	// titleBarのカラーを設定
	SetTitleBar(ImColor(255, 99, 71));

	// parameterの初期化
	param_->dissolveColor = Colors::Linear::white;
	param_->threshold = 0.1f;
	param_->edgeWidth = 1.0f;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　更新関数
//////////////////////////////////////////////////////////////////////////////////////////////////

void DissolveNode::customUpdate() {
	inputBaseResource_ = getInVal<AOENGINE::DxResource*>("BaseTex");
	inputNoiseResource_ = getInVal<AOENGINE::DxResource*>("NoiseTex");
	for (auto& connection : this->getIns()) {
		if (connection->getName() == "threshold") {
			if (connection->isConnected()) {
				threshold_ = getInVal<float>("threshold");
			}
		}
	}
	
	param_->threshold = threshold_;

	// resourceの作成
	CreateView();

	// csの実行
	ExecuteCommand();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　Guiの更新
//////////////////////////////////////////////////////////////////////////////////////////////////

void DissolveNode::updateGui() {
	ImGui::ColorEdit4("color", &param_->dissolveColor.r);
	ImGui::DragFloat("threshold", &threshold_, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat("edgeWidth", &param_->edgeWidth, 0.1f);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　Nodeの描画
//////////////////////////////////////////////////////////////////////////////////////////////////

void DissolveNode::draw() {
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

nlohmann::json DissolveNode::toJson() {
	nlohmann::json result;
	BaseInfoToJson(result);
	result["props"]["dissolveColor"] = Convert::toJson<Color>(param_->dissolveColor);
	result["props"]["threshold"] = Convert::toJson<float>(threshold_);
	result["props"]["edgeWidth"] = Convert::toJson<float>(param_->edgeWidth);
	return result;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　jsonから復元
//////////////////////////////////////////////////////////////////////////////////////////////////

void DissolveNode::fromJson(const nlohmann::json& _json) {
	BaseInfoFromJson(_json);
	param_->dissolveColor.r = _json["props"]["dissolveColor"]["r"].get<float>();
	param_->dissolveColor.g = _json["props"]["dissolveColor"]["g"].get<float>();
	param_->dissolveColor.b = _json["props"]["dissolveColor"]["b"].get<float>();
	param_->dissolveColor.a = _json["props"]["dissolveColor"]["a"].get<float>();
	threshold_ = _json["props"]["threshold"].get<float>();
	param_->edgeWidth = _json["props"]["edgeWidth"].get<float>();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　viewの作成
//////////////////////////////////////////////////////////////////////////////////////////////////

void DissolveNode::CreateView() {
	if (inputBaseResource_) {
		if (!outputResource_->GetResource()) {
			CreteOutputResource(outputResource_, inputBaseResource_);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　commandの実行
//////////////////////////////////////////////////////////////////////////////////////////////////

void DissolveNode::ExecuteCommand() {
	if (inputBaseResource_ && inputNoiseResource_) {
		if (outputResource_->GetResource()) {
			outputResource_->Transition(cmdList_, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

			Pipeline* pso = Engine::SetPipelineCS("Dissolve.json");
			UINT index = 0;
			index = pso->GetRootSignatureIndex("gBaseTex");
			cmdList_->SetComputeRootDescriptorTable(index, inputBaseResource_->GetSRV().handleGPU);
			index = pso->GetRootSignatureIndex("gNoiseTex");
			cmdList_->SetComputeRootDescriptorTable(index, inputNoiseResource_->GetSRV().handleGPU);
			index = pso->GetRootSignatureIndex("gOutputTex");
			cmdList_->SetComputeRootDescriptorTable(index, outputResource_->GetUAV().handleGPU);
			index = pso->GetRootSignatureIndex("gDissolve");
			cmdList_->SetComputeRootConstantBufferView(index, buffer_->GetGPUVirtualAddress());
			cmdList_->Dispatch(UINT(outputResource_->GetDesc()->Width / 16), UINT(outputResource_->GetDesc()->Height / 16), 1);

			outputResource_->Transition(cmdList_, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
		}
	}
}
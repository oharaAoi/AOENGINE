#include "DissolveNode.h"
#include "Engine/Engine.h"
#include "Engine/Lib/GameTimer.h"

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

	// paremterのバッファを確保
	buffer_ = CreateBufferResource(ctx_->GetDevice(), sizeof(DissolveParams));
	buffer_->Map(0, nullptr, reinterpret_cast<void**>(&param_));

	outputResource_ = ctx_->CreateDxResource(ResourceType::Common);
	auto texOut = addOUT<AOENGINE::DxResource*>("DxResource", ImFlow::PinStyle::green());
	texOut->behaviour([this]() { return outputResource_; });
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　更新関数
//////////////////////////////////////////////////////////////////////////////////////////////////

void DissolveNode::customUpdate() {
	inputBaseResource_ = getInVal<AOENGINE::DxResource*>("BaseTex");
	inputNoiseResource_ = getInVal<AOENGINE::DxResource*>("NoiseTex");

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
	ImGui::DragFloat("threshold", &param_->threshold, 0.01f, 0.0f, 1.0f);
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
	result["props"]["threshold"] = Convert::toJson<float>(param_->threshold);
	result["props"]["edgeWidth"] = Convert::toJson<float>(param_->edgeWidth);
	return result;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　jsonから復元
//////////////////////////////////////////////////////////////////////////////////////////////////

void DissolveNode::fromJson(const nlohmann::json& _json) {
	BaseInfoFromJson(_json);
	Convert::fromJson(_json, "dissolveColor", param_->dissolveColor);
	Convert::fromJson(_json, "threshold", param_->threshold);
	Convert::fromJson(_json, "edgeWidth", param_->edgeWidth);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　viewの作成
//////////////////////////////////////////////////////////////////////////////////////////////////

void DissolveNode::CreateView() {
	if (inputBaseResource_) {
		if (!outputResource_->GetResource()) {
			D3D12_RESOURCE_DESC desc = *inputBaseResource_->GetDesc();
			desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
			desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			const D3D12_HEAP_PROPERTIES heapProperties{ .Type = D3D12_HEAP_TYPE_DEFAULT };
			outputResource_->CreateResource(&desc, &heapProperties, D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);

			// ------------------------------------------------------------
			// UAVの設定
			D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
			uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
			uavDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			outputResource_->CreateUAV(uavDesc);

			// ------------------------------------------------------------
			// SRVの設定
			D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
			srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MipLevels = 1;
			outputResource_->CreateSRV(srvDesc);
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
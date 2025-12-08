#include "DistortionNode.h"
#include "Engine/Engine.h"
#include "Engine/Lib/GameTimer.h"

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

	// paremterのバッファを確保
	buffer_ = CreateBufferResource(ctx_->GetDevice(), sizeof(DistortionParam));
	buffer_->Map(0, nullptr, reinterpret_cast<void**>(&param_));

	outputResource_ = ctx_->CreateDxResource(ResourceType::Common);
	auto texOut = addOUT<AOENGINE::DxResource*>("DxResource", ImFlow::PinStyle::green());
	texOut->behaviour([this]() { return outputResource_; });
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
	ImGui::DragFloat("strength", &param_->strength, 0.1f);
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
	result["props"]["strength"] = Convert::toJson<float>(param_->strength);
	return result;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　json形式から変換
//////////////////////////////////////////////////////////////////////////////////////////////////

void DistortionNode::fromJson(const nlohmann::json& _json) {
	BaseInfoFromJson(_json);
	param_->scroll.x = _json["props"]["scroll"]["x"].get<float>();
	param_->scroll.y = _json["props"]["scroll"]["y"].get<float>();
	param_->strength = _json["props"]["strength"].get<float>();
	param_->tiling.x = _json["props"]["tiling"]["x"].get<float>();
	param_->tiling.y = _json["props"]["tiling"]["y"].get<float>();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　viewの作成
//////////////////////////////////////////////////////////////////////////////////////////////////

void DistortionNode::CreateView() {
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
			cmdList_->Dispatch(UINT(outputResource_->GetDesc()->Width / 16), UINT(outputResource_->GetDesc()->Height / 16), 1);

			outputResource_->Transition(cmdList_, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
		}
	}
}

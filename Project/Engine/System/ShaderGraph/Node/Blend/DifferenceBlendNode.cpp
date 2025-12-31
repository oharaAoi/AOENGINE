#include "DifferenceBlendNode.h"
#include "Engine/Engine.h"
#include "Engine/System/ShaderGraph/ShaderGraphHelperFunc.h"
#include "Engine/Lib/CalcDispatchSize2D.h"

using namespace AOENGINE;

DifferenceBlendNode::DifferenceBlendNode() {}
DifferenceBlendNode::~DifferenceBlendNode() {
	resourceA_ = nullptr;
	resourceB_ = nullptr;
	blendResource_->Destroy();
}

void DifferenceBlendNode::Init() {
	ctx_ = AOENGINE::GraphicsContext::GetInstance();
	cmdList_ = ctx_->GetCommandList();

	blendResource_ = ctx_->CreateDxResource(ResourceType::Common);

	// inputの設定
	addIN<AOENGINE::DxResource*>("TextureA", nullptr, ImFlow::ConnectionFilter::SameType());
	addIN<AOENGINE::DxResource*>("TextureB", nullptr, ImFlow::ConnectionFilter::SameType());

	// outputの設定
	auto texOut = addOUT<AOENGINE::DxResource*>("DxResource", ImFlow::PinStyle::green());
	texOut->behaviour([this]() { return blendResource_; });

	// titleBarのカラーを設定
	SetTitleBar(ImColor(220, 20, 60));
}

void DifferenceBlendNode::customUpdate() {
	// 入力からの受け取り
	resourceA_ = getInVal<AOENGINE::DxResource*>("TextureA");
	resourceB_ = getInVal<AOENGINE::DxResource*>("TextureB");

	// 入力があるのなら合成結果のresourceを作成する
	CreateBlendResource();

	if (resourceA_ && resourceB_) {
		if (blendResource_->GetResource()) {
			blendResource_->Transition(cmdList_, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

			Pipeline* pso = Engine::SetPipelineCS("DifferenceBlend.json");
			UINT index = 0;
			index = pso->GetRootSignatureIndex("texA");
			cmdList_->SetComputeRootDescriptorTable(index, resourceA_->GetSRV().handleGPU);
			index = pso->GetRootSignatureIndex("texB");
			cmdList_->SetComputeRootDescriptorTable(index, resourceB_->GetSRV().handleGPU);
			index = pso->GetRootSignatureIndex("outputTex");
			cmdList_->SetComputeRootDescriptorTable(index, blendResource_->GetUAV().handleGPU);
			auto desc = blendResource_->GetDesc();
			ComputeDispatchSize2D group = CalcDispatchSize(*desc);
			cmdList_->Dispatch(group.x, group.y, 1);
			blendResource_->Transition(cmdList_, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
		}
	}
}

void DifferenceBlendNode::draw() {
	// 合成結果の描画
	if (blendResource_->GetResource()) {
		if (resourceA_ && resourceB_) {
			ImTextureID texID = (ImTextureID)(intptr_t)(blendResource_->GetSRV().handleGPU.ptr);
			ImGui::SetNextWindowBgAlpha(0.85f);
			ImGui::Image(texID, ImVec2(64, 64));
		}
	}
}

nlohmann::json DifferenceBlendNode::toJson() {
	nlohmann::json result;
	BaseInfoToJson(result);
	return result;
}

void DifferenceBlendNode::fromJson(const nlohmann::json& _json) {
	BaseInfoFromJson(_json);
}

void DifferenceBlendNode::CreateBlendResource() {
	if (resourceA_) {
		if (!blendResource_->GetResource()) {
			CreteOutputResource(blendResource_, resourceA_);
		}
	}
}

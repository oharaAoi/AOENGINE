#include "MaskBlendNode.h"
#include "Engine/Engine.h"
#include "Engine/System/ShaderGraph/ShaderGraphHelperFunc.h"

using namespace AOENGINE;

MaskBlendNode::~MaskBlendNode() {
	blendResource_->Destroy();
}

void MaskBlendNode::Init() {
	ctx_ = AOENGINE::GraphicsContext::GetInstance();
	cmdList_ = ctx_->GetCommandList();

	blendResource_ = ctx_->CreateDxResource(ResourceType::Common);
	
    // inputの設定
	addIN<AOENGINE::DxResource*>("TextureA", nullptr, ImFlow::ConnectionFilter::SameType());
	addIN<AOENGINE::DxResource*>("TextureB", nullptr, ImFlow::ConnectionFilter::SameType());

    // outputの設定
    auto texOut = addOUT<AOENGINE::DxResource*>("Texture", ImFlow::PinStyle::green());
    texOut->behaviour([this]() { return blendResource_; });
    // titleBarのカラーを設定
    SetTitleBar(ImColor(220, 20, 60));
}

void MaskBlendNode::customUpdate() {
    // 入力の受取
    resourceA_ = getInVal<AOENGINE::DxResource*>("TextureA");
    resourceB_ = getInVal<AOENGINE::DxResource*>("TextureB");

    if (resourceA_) {
        if (!blendResource_->GetResource()) {
            CreteOutputResource(blendResource_, resourceA_);
        }
    }

    if (resourceA_ && resourceB_) {
        if (blendResource_->GetResource()) {
            blendResource_->Transition(cmdList_, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

            Pipeline* pso = Engine::SetPipelineCS("MaskBlend.json");
            UINT index = 0;
            index = pso->GetRootSignatureIndex("texA");
            cmdList_->SetComputeRootDescriptorTable(index, resourceA_->GetSRV().handleGPU);
            index = pso->GetRootSignatureIndex("texB");
            cmdList_->SetComputeRootDescriptorTable(index, resourceB_->GetSRV().handleGPU);
            index = pso->GetRootSignatureIndex("outputTex");
            cmdList_->SetComputeRootDescriptorTable(index, blendResource_->GetUAV().handleGPU);
            cmdList_->Dispatch(UINT(blendResource_->GetDesc()->Width / 16), UINT(blendResource_->GetDesc()->Height / 16), 1);

            blendResource_->Transition(cmdList_, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
        }
    }
}

void MaskBlendNode::draw() {
    if (blendResource_->GetResource()) {
        if (resourceA_ && resourceB_) {
            ImTextureID texID = (ImTextureID)(intptr_t)(blendResource_->GetSRV().handleGPU.ptr);
            ImGui::SetNextWindowBgAlpha(0.85f);
            ImGui::Image(texID, ImVec2(64, 64));
        }
    }
}

nlohmann::json MaskBlendNode::toJson() {
    nlohmann::json result;
    BaseInfoToJson(result);
    return result;
}

void MaskBlendNode::fromJson(const nlohmann::json& _json) {
    BaseInfoFromJson(_json);
}

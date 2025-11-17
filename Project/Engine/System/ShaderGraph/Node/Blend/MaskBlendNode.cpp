#include "MaskBlendNode.h"
#include "Engine/Engine.h"

MaskBlendNode::~MaskBlendNode() {
	blendResource_->Destroy();
}

void MaskBlendNode::Init() {
	ctx_ = GraphicsContext::GetInstance();
	cmdList_ = ctx_->GetCommandList();

	blendResource_ = ctx_->CreateDxResource(ResourceType::COMMON);
	
    // inputの設定
	addIN<DxResource*>("TextureA", nullptr, ImFlow::ConnectionFilter::SameType());
	addIN<DxResource*>("TextureB", nullptr, ImFlow::ConnectionFilter::SameType());

    // outputの設定
    auto texOut = addOUT<DxResource*>("Texture", ImFlow::PinStyle::green());
    texOut->behaviour([this]() { return blendResource_; });
}

void MaskBlendNode::customUpdate() {
    // 入力の受取
    resourceA_ = getInVal<DxResource*>("TextureA");
    resourceB_ = getInVal<DxResource*>("TextureB");

    if (resourceA_) {
        if (!blendResource_->GetResource()) {
            D3D12_RESOURCE_DESC desc = *resourceA_->GetDesc();
            desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
            desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            const D3D12_HEAP_PROPERTIES heapProperties{ .Type = D3D12_HEAP_TYPE_DEFAULT };
            blendResource_->CreateResource(&desc, &heapProperties, D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);

            // ------------------------------------------------------------
            // UAVの設定
            D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
            uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
            uavDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            blendResource_->CreateUAV(uavDesc);

            // ------------------------------------------------------------
            // SRVの設定
            D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
            srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
            srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
            srvDesc.Texture2D.MipLevels = 1;
            blendResource_->CreateSRV(srvDesc);
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

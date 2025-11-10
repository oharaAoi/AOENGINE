#include "SampleTexture2dNode.h"
#include "Engine/Engine.h"

SampleTexture2dNode::SampleTexture2dNode() { }
SampleTexture2dNode::~SampleTexture2dNode() {
    resource_->Finalize();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化関数
///////////////////////////////////////////////////////////////////////////////////////////////

void SampleTexture2dNode::Init() {
    ctx_ = GraphicsContext::GetInstance();
    cmdList_ = ctx_->GetCommandList();

    resource_ = std::make_unique<DxResource>();
    resource_->Init(ctx_->GetDevice(), ctx_->GetDxHeap(), ResourceType::COMMON);

    uvBuffer_ = CreateBufferResource(ctx_->GetDevice(), sizeof(UVParam));
    uvBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&uvParam_));

    addIN<std::shared_ptr<DxResource*>>("Texture", nullptr, ImFlow::ConnectionFilter::None());
    addIN<Vector2>("UV", uvParam_->uv, ImFlow::ConnectionFilter::None());
}

void SampleTexture2dNode::customUpdate() {
    if (inputResource_) {
        if (resource_->GetResource()) {
            resource_->Transition(cmdList_, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

            Pipeline* pso = Engine::SetPipelineCS("SampleTexture.json");
            UINT index = 0;
            index = pso->GetRootSignatureIndex("gInputTex");
            cmdList_->SetComputeRootDescriptorTable(index, inputResource_->GetSRV().handleGPU);
            index = pso->GetRootSignatureIndex("gUV");
            cmdList_->SetComputeRootConstantBufferView(index, uvBuffer_->GetGPUVirtualAddress());
            index = pso->GetRootSignatureIndex("gPreviewTex");
            cmdList_->SetComputeRootDescriptorTable(index, resource_->GetUAV().handleGPU);
            cmdList_->Dispatch(UINT(resource_->GetDesc()->Width / 16), UINT(resource_->GetDesc()->Height / 16), 1);

            resource_->Transition(cmdList_, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ node描画
///////////////////////////////////////////////////////////////////////////////////////////////

void SampleTexture2dNode::draw() {
    // -------- 入力ピン ----------
    inputResource_ = getInVal<DxResource*>("Texture");
    uvParam_->uv = getInVal<Vector2>("UV");

    if (inputResource_) {
        if (!resource_->GetResource()) {
            D3D12_RESOURCE_DESC desc = *inputResource_->GetDesc();
            desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
            desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            const D3D12_HEAP_PROPERTIES heapProperties{ .Type = D3D12_HEAP_TYPE_DEFAULT };
            resource_->CreateResource(&desc, &heapProperties, D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);

            // ------------------------------------------------------------
            // UAVの設定
            D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
            uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
            uavDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            resource_->CreateUAV(uavDesc);

            // ------------------------------------------------------------
            // SRVの設定
            D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
            srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
            srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
            srvDesc.Texture2D.MipLevels = 1;
            resource_->CreateSRV(srvDesc);
        }
    }
    // -------- 出力ピン ----------
    showOUT<DxResource*>(
        "DxResource",
        [=]() -> DxResource* {
            // 簡易的なカラー出力 (本来はGPUサンプリング)
            return resource_.get();
        },
        ImFlow::PinStyle::green()
    );

    // -------- 内部プレビュー ----------
    if (resource_->GetResource()) {
        if (inputResource_) {
            ImTextureID texID = (ImTextureID)(intptr_t)(resource_->GetSRV().handleGPU.ptr);
            ImGui::SetNextWindowBgAlpha(0.85f);
            ImGui::Image(texID, ImVec2(64, 64));
        }
    }
}

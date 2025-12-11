#include "SampleTexture2dNode.h"
#include "Engine/Engine.h"
#include "Engine/System/ShaderGraph/ShaderGraphHelperFunc.h"

using namespace AOENGINE;

SampleTexture2dNode::SampleTexture2dNode() { }
SampleTexture2dNode::~SampleTexture2dNode() {
    resource_->Destroy();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化関数
///////////////////////////////////////////////////////////////////////////////////////////////

void SampleTexture2dNode::Init() {
    ctx_ = AOENGINE::GraphicsContext::GetInstance();
    cmdList_ = ctx_->GetCommandList();

    resource_ = ctx_->CreateDxResource(ResourceType::Common);
    
    uvBuffer_ = CreateBufferResource(ctx_->GetDevice(), sizeof(NodeUVTransform));
    uvBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&uvParam_));

    colorBuffer_ = CreateBufferResource(ctx_->GetDevice(), sizeof(AOENGINE::Color));
    colorBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&colorParam_));

    // inputの設定
    addIN<std::shared_ptr<AOENGINE::DxResource*>>("Texture", nullptr, ImFlow::ConnectionFilter::None());
    addIN<NodeUVTransform>("UV", uv_, ImFlow::ConnectionFilter::None());

    // outputの設定
    auto texOut = addOUT<AOENGINE::DxResource*>("DxResource", ImFlow::PinStyle::green());
    texOut->behaviour([this]() { return resource_; });

    // titleBarのカラーを設定
    SetTitleBar(ImColor(46, 139, 87));

    *colorParam_ = Colors::Linear::white;
    colorParam_->a = 0.0f;
}

void SampleTexture2dNode::customUpdate() {
    // 入力の受取
    inputResource_ = getInVal<AOENGINE::DxResource*>("Texture");
    uv_ = getInVal<NodeUVTransform>("UV");
    uvParam_->scale = uv_.scale;
    uvParam_->rotate = uv_.rotate;
    uvParam_->translate = uv_.translate;

    CreateView();

    // textureの合成
    if (inputResource_) {
        if (resource_->GetResource()) {
            resource_->Transition(cmdList_, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

            Pipeline* pso = Engine::SetPipelineCS("SampleTexture.json");
            UINT index = 0;
            index = pso->GetRootSignatureIndex("gInputTex");
            cmdList_->SetComputeRootDescriptorTable(index, inputResource_->GetSRV().handleGPU);
            index = pso->GetRootSignatureIndex("gUV");
            cmdList_->SetComputeRootConstantBufferView(index, uvBuffer_->GetGPUVirtualAddress());
            index = pso->GetRootSignatureIndex("gColor");
            cmdList_->SetComputeRootConstantBufferView(index, colorBuffer_->GetGPUVirtualAddress());
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
    // -------- 内部プレビュー ----------
    if (resource_->GetResource()) {
        if (inputResource_) {
            ImTextureID texID = (ImTextureID)(intptr_t)(resource_->GetSRV().handleGPU.ptr);
            ImGui::SetNextWindowBgAlpha(0.85f);
            ImGui::Image(texID, ImVec2(64, 64));
            ImGui::SetNextItemWidth(150);
            ImGui::ColorEdit4("color", &colorParam_->r);
        }
    }
}

nlohmann::json SampleTexture2dNode::toJson() {
    nlohmann::json result;
    BaseInfoToJson(result);
    result["props"]["color"] = Convert::toJson<Color>(AOENGINE::Color(colorParam_->r, colorParam_->g, colorParam_->b, colorParam_->a ));
    return result;
}

void SampleTexture2dNode::fromJson(const nlohmann::json& _json) {
    BaseInfoFromJson(_json);

    if (_json.contains("props")) {
        colorParam_->r = _json["props"]["color"].value("r", colorParam_->r);
        colorParam_->g = _json["props"]["color"].value("g", colorParam_->g);
        colorParam_->b = _json["props"]["color"].value("b", colorParam_->b);
        colorParam_->a = _json["props"]["color"].value("a", colorParam_->a);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ viewの作成
///////////////////////////////////////////////////////////////////////////////////////////////

void SampleTexture2dNode::CreateView() {
    if (inputResource_) {
        if (!resource_->GetResource()) {
            CreteOutputResource(resource_, inputResource_);
        }
    }
}
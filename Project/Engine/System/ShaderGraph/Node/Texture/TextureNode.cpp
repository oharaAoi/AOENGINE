#include "TextureNode.h"
#include "Engine/System/Manager/TextureManager.h"

using namespace AOENGINE;

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化関数
///////////////////////////////////////////////////////////////////////////////////////////////

TextureNode::~TextureNode() {
    resource_ = nullptr;
}

void TextureNode::Init() {
    auto texOut = addOUT<AOENGINE::DxResource*>("DxResource", ImFlow::PinStyle::green());
    texOut->behaviour([this]() { return resource_; });

    // titleBarのカラーを設定
    SetTitleBar(ImColor(46, 139, 87));
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新関数
///////////////////////////////////////////////////////////////////////////////////////////////

void TextureNode::updateGui() {
    SelectTexture();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ node描画
///////////////////////////////////////////////////////////////////////////////////////////////

void TextureNode::draw() {
    // -------- 内部プレビュー ----------
    if (textureName_ != "") {
        if (resource_) {
            ImTextureID texID = (ImTextureID)(intptr_t)(resource_->GetSRV().handleGPU.ptr);
            ImGui::SetNextWindowBgAlpha(0.85f);
            ImGui::Image(texID, ImVec2(64, 64));
        }
    }
}

nlohmann::json TextureNode::toJson() {
    nlohmann::json result;
    BaseInfoToJson(result);
    result["props"]["textureName"] = textureName_;
    return result;
}

void TextureNode::fromJson(const nlohmann::json& _json) {
    BaseInfoFromJson(_json);
    textureName_ = _json.at("props").at("textureName").get<std::string>();
    resource_ = AOENGINE::TextureManager::GetInstance()->GetResource(textureName_);
}

void TextureNode::SelectTexture() {
    if (AOENGINE::TextureManager::GetInstance()->PreviewTexture(textureName_)) {
        resource_ = AOENGINE::TextureManager::GetInstance()->GetResource(textureName_);
    }
}

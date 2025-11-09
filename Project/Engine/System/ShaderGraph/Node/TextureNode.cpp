#include "TextureNode.h"
#include "Engine/System/Manager/TextureManager.h"

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化関数
///////////////////////////////////////////////////////////////////////////////////////////////

TextureNode::~TextureNode() {
    resource_ = nullptr;
}

void TextureNode::Init() {
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
    // -------- 出力ピン ----------
    showOUT<DxResource*>(
        "Texture",
        [=]() -> DxResource* {
            // 簡易的なカラー出力 (本来はGPUサンプリング)
            return resource_;
        },
        ImFlow::PinStyle::green()
    );

    // -------- 内部プレビュー ----------
    if (textureName_ != "") {
        ImTextureID texID = (ImTextureID)(intptr_t)(resource_->GetSRV().handleGPU.ptr);
        ImGui::SetNextWindowBgAlpha(0.85f);
        ImGui::Image(texID, ImVec2(64, 64));
    }
}

void TextureNode::SelectTexture() {
    if (TextureManager::GetInstance()->PreviewTexture(textureName_)) {
        resource_ = TextureManager::GetInstance()->GetResource(textureName_);
    }
}

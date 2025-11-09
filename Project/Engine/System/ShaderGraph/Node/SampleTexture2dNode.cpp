#include "SampleTexture2dNode.h"

SampleTexture2dNode::SampleTexture2dNode() { }
SampleTexture2dNode::~SampleTexture2dNode() {}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化関数
///////////////////////////////////////////////////////////////////////////////////////////////

void SampleTexture2dNode::Init() {
    addIN<std::shared_ptr<DxResource*>>("Texture", nullptr, ImFlow::ConnectionFilter::None());
    addIN<std::shared_ptr<DxResource*>>("UV", nullptr, ImFlow::ConnectionFilter::None());
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ node描画
///////////////////////////////////////////////////////////////////////////////////////////////

void SampleTexture2dNode::draw() {
    // -------- 入力ピン ----------
    resource_ = showIN<DxResource*>(
        "Texture",
        nullptr,
        ImFlow::ConnectionFilter::SameType(),
        ImFlow::PinStyle::green()
    );

    uv_ = showIN<Vector2>(
        "UV",
        Vector2(0.0f, 0.0f),
        ImFlow::ConnectionFilter::SameType(),
        ImFlow::PinStyle::blue()
    );

    // -------- 出力ピン ----------
    showOUT<Color>(
        "Color",
        [=]() -> Color {
            // 簡易的なカラー出力 (本来はGPUサンプリング)
            return Color(uv_.x, uv_.y, 1.0f - uv_.x, 1.0f);
        },
        ImFlow::PinStyle::green()
    );

    // -------- 内部プレビュー ----------
    if (resource_) {
        ImTextureID texID = (ImTextureID)(intptr_t)(resource_->GetSRV().handleGPU.ptr);
        ImGui::SetNextWindowBgAlpha(0.85f);
        ImGui::Image(texID, ImVec2(64, 64));
    }
}

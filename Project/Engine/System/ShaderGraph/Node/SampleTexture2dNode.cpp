#include "SampleTexture2dNode.h"

SampleTexture2dNode::SampleTexture2dNode() { }
SampleTexture2dNode::~SampleTexture2dNode() {}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化関数
///////////////////////////////////////////////////////////////////////////////////////////////

void SampleTexture2dNode::Init() {
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新関数
///////////////////////////////////////////////////////////////////////////////////////////////

void SampleTexture2dNode::Update() {
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ node描画
///////////////////////////////////////////////////////////////////////////////////////////////

void SampleTexture2dNode::draw() {
    // -------- 入力ピン ----------
    textureID_ = showIN<ImTextureID>(
        "Texture",
        nullptr,
        ImFlow::ConnectionFilter::None(),
        ImFlow::PinStyle::white()
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
    if (textureID_) {
        ImGui::Image(textureID_, ImVec2(64, 64));
    } else {
        ImGui::Dummy(ImVec2(64, 64)); // placeholder
    }
}

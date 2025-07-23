#include "PostureStability.h"
#include "Engine.h"
#include "Engine/Lib/Json/JsonItems.h"

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void PostureStability::Init() {
	BaseGaugeUI::Init("postureStability_bg.png", "postureStability_front.png");

	fence_ = Engine::CreateSprite("postureStability_fence.png");

	param_.FromJson(JsonItems::GetData(GetName(), param_.GetName()));

	fence_->SetTranslate(centerPos_);
	fence_->SetScale(scale_);

	fillMoveType_ = 1;

	Engine::GetCanvas2d()->AddSprite(fence_.get());
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void PostureStability::Update(float _fillAmount) {
	fillAmount_ = _fillAmount;
	Vector4 color = Vector4::Lerp(param_.normalColor, param_.pinchColor, fillAmount_);
	front_->SetColor(color);

	BaseGaugeUI::Update();

	fence_->SetTranslate(centerPos_);
	fence_->SetScale(scale_);

	fence_->Update();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 描画処理
///////////////////////////////////////////////////////////////////////////////////////////////

void PostureStability::Draw() const {
	BaseGaugeUI::Draw();

	Pipeline* pso = Engine::GetLastUsedPipeline();
	fence_->Draw(pso);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集処理
///////////////////////////////////////////////////////////////////////////////////////////////

void PostureStability::Debug_Gui() {
	BaseGaugeUI::Debug_Gui();

	if (ImGui::CollapsingHeader("Parameter")) {
		ImGui::ColorEdit4("normalColor", &param_.normalColor.x);
		ImGui::ColorEdit4("pinchColor", &param_.pinchColor.x);

		if (ImGui::Button("Save")) {
			JsonItems::Save(GetName(), param_.ToJson(param_.GetName()));
		}
	}
}
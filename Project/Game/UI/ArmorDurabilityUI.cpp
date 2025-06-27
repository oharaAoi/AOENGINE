#include "ArmorDurabilityUI.h"
#include "Engine.h"

void ArmorDurabilityUI::Init() {
	BaseGaugeUI::Init("postureStability_bg.png", "pulseArmorGauge.png");

	fence_ = Engine::CreateSprite("postureStability_fence.png");

	fence_->SetTranslate(centerPos_);
	fence_->SetScale(scale_);

	fillMoveType_ = 1;
}

void ArmorDurabilityUI::Update(float _fillAmount) {
	fillAmount_ = _fillAmount;
	BaseGaugeUI::Update();

	fence_->SetTranslate(centerPos_);
	fence_->SetScale(scale_);

	fence_->Update();
}

void ArmorDurabilityUI::Draw() const {
	BaseGaugeUI::Draw();

	Pipeline* pso = Engine::GetLastUsedPipeline();
	fence_->Draw(pso);
}

void ArmorDurabilityUI::Debug_Gui() {
	BaseGaugeUI::Debug_Gui();
}

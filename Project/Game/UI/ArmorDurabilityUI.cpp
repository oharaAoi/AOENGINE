#include "ArmorDurabilityUI.h"
#include "Engine.h"

void ArmorDurabilityUI::Init(const std::string& _groupName, const std::string& _itemName) {
	SetName("ArmorDurabilityUI");
	groupName_ = _groupName;
	BaseGaugeUI::Init("postureStability_bg.png", "pulseArmorGauge.png");
	front_->Load(_groupName, _itemName);

	// 柵の初期化
	fence_ = Engine::GetCanvas2d()->AddSprite("postureStability_fence.png", "fence");
	fence_->SetTranslate(front_->GetTranslate());
	fence_->SetScale(front_->GetScale());
}

void ArmorDurabilityUI::Update(float _fillAmount) {
	fillAmount_ = _fillAmount;
	BaseGaugeUI::Update();
	fence_->Update();
}

void ArmorDurabilityUI::Debug_Gui() {
	BaseGaugeUI::Debug_Gui();
}

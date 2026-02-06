#include "PostureStability.h"
#include "Engine.h"
#include "Engine/Lib/Json/JsonItems.h"
#include "Engine/Lib/GameTimer.h"

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void PostureStability::Init(const std::string& _groupName, const std::string& _itemName) {
	SetName("PostureStability");
	param_.SetGroupName(_groupName);
	param_.Load();

	groupName_ = _groupName;
	BaseGaugeUI::Init("postureStability_bg.png", "postureStability_front.png");
	front_->Load(_groupName, _itemName);

	// 柵の初期化
	fence_ = Engine::GetCanvas2d()->AddSprite("postureStability_fence.png", "fence");
	fence_->SetTranslate(front_->GetTranslate());
	fence_->SetScale(front_->GetScale());

	AOENGINE::Color color = param_.stunColor;
	color.a = 0.0f;
	stunAnimation_.Init(param_.stunColor, color, 0.2f, (int)EasingType::None::Liner, LoopType::Return);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void PostureStability::Update(float _fillAmount) {
	fillAmount_ = _fillAmount;

	// タイプによってカラーを変える
	switch (gaugeType_) {
	case GaugeType::Posturebility:
	{
		AOENGINE::Color color = AOENGINE::Color::Lerp(param_.normalColor, param_.pinchColor, fillAmount_);
		front_->SetColor(color);
	}
		break;
	case GaugeType::Armor:
		front_->SetColor(param_.armorColor);
		break;
	case GaugeType::Stun:
		stunAnimation_.Update(AOENGINE::GameTimer::DeltaTime());
		front_->SetColor(stunAnimation_.GetValue());
		break;
	default:
		break;
	}

	BaseGaugeUI::Update();

	fence_->SetEnable(this->GetIsEnable());
	fence_->SetTranslate(front_->GetTranslate());
	fence_->SetScale(front_->GetScale());
	fence_->Update();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集処理
///////////////////////////////////////////////////////////////////////////////////////////////

void PostureStability::Debug_Gui() {
	BaseGaugeUI::Debug_Gui();

	if (ImGui::CollapsingHeader("Parameter")) {
		ImGui::ColorEdit4("normalColor", &param_.normalColor.r);
		ImGui::ColorEdit4("pinchColor", &param_.pinchColor.r);
		ImGui::ColorEdit4("armorColor", &param_.armorColor.r);
		ImGui::ColorEdit4("stunColor", &param_.stunColor.r);

		param_.SaveAndLoad();
	}
}
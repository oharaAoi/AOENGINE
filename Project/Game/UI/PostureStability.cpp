#include "PostureStability.h"
#include "Engine.h"
#include "Engine/Lib/Json/JsonItems.h"

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void PostureStability::Init(const std::string& _groupName, const std::string& _itemName) {
	SetName("PostureStability");
	param_.FromJson(JsonItems::GetData("PostureStability", param_.GetName()));

	groupName_ = _groupName;
	BaseGaugeUI::Init("postureStability_bg.png", "postureStability_front.png");
	front_->Load(_groupName, _itemName);

	// 柵の初期化
	fence_ = Engine::GetCanvas2d()->AddSprite("postureStability_fence.png", "fence");
	fence_->SetTranslate(front_->GetTranslate());
	fence_->SetScale(front_->GetScale());

	Color color = param_.stanColor;
	color.a = 0.0f;
	stanAnimation_.Init(param_.stanColor, color, 0.2f, (int)EasingType::None::Liner, LoopType::RETURN);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void PostureStability::Update(float _fillAmount) {
	fillAmount_ = _fillAmount;

	switch (gaugeType_) {
	case GaugeType::Posturebility:
	{
		Color color = Color::Lerp(param_.normalColor, param_.pinchColor, fillAmount_);
		front_->SetColor(color);
	}
		break;
	case GaugeType::Armor:
		front_->SetColor(param_.armorColor);
		break;
	case GaugeType::Stan:
		stanAnimation_.Update(GameTimer::DeltaTime());
		front_->SetColor(stanAnimation_.GetValue());
		break;
	default:
		break;
	}

	BaseGaugeUI::Update();

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
		ImGui::ColorEdit4("stanColor", &param_.stanColor.r);

		if (ImGui::Button("Save")) {
			JsonItems::Save(GetName(), param_.ToJson(param_.GetName()));
		}
	}
}
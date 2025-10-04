#include "StanGaugeUI.h"
#include "Engine.h"
#include "Engine/Lib/Json/JsonItems.h"

void StanGaugeUI::Init(const std::string& _groupName, const std::string& _itemName) {
	SetName("StanGaugeUI");
	groupName_ = _groupName;
	param_.FromJson(JsonItems::GetData("UI", param_.GetName()));

	BaseGaugeUI::Init("postureStability_bg.png", "gauge_front.png");
	front_->Load(_groupName, _itemName);

	scaleTween_.Init((front_->GetScale() * 1.2f), front_->GetScale(), 0.2f, (int)EasingType::InOut::Sine, LoopType::STOP);
	alphaTween_.Init(param_.minAlpha, param_.maxAlpha, param_.popInterval, (int)EasingType::InOut::Sine, LoopType::RETURN);
}

void StanGaugeUI::Update(float _fillAmount) {
	fillAmount_ = _fillAmount;
	scaleTween_.Update(GameTimer::DeltaTime());
	alphaTween_.Update(GameTimer::DeltaTime());

	front_->SetScale(scaleTween_.GetValue());
	front_->SetColor(Color(1.0f, 0.2f, 0.2f, alphaTween_.GetValue()));

	BaseGaugeUI::Update();
}

void StanGaugeUI::Debug_Gui() {
	BaseGaugeUI::Debug_Gui();

	ImGui::DragFloat("popInterval", &param_.popInterval, 0.1f);
	ImGui::DragFloat("minAlpha", &param_.minAlpha, 0.1f);
	ImGui::DragFloat("maxAlpha", &param_.maxAlpha, 0.1f);

	if (ImGui::Button("Save")) {
		JsonItems::Save("UI", param_.ToJson(param_.GetName()));
	}
}

void StanGaugeUI::Pop() {
	scaleTween_.Reset();
	alphaTween_.Reset();
}

void StanGaugeUI::SetIsEnable(bool _isActive) {
	front_->SetEnable(_isActive);
	bg_->SetEnable(_isActive);
}
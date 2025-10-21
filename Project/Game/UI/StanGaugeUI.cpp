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
}

void StanGaugeUI::Update(float _fillAmount) {
	fillAmount_ = _fillAmount;
	scaleTween_.Update(GameTimer::DeltaTime());
	colorTween_.Update(GameTimer::DeltaTime());
	
	front_->SetScale(scaleTween_.GetValue());
	Color color = colorTween_.GetValue();
	front_->SetColor(color);

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
	colorTween_.Reset();
}

void StanGaugeUI::SetIsEnable(bool _isActive) {
	front_->SetEnable(_isActive);
	bg_->SetEnable(_isActive);

	if (_isActive) {
		colorTween_.Init(Color(1.f, 0.2f, 0.2f, 1.0f), Color(1.f, 0.2f, 0.2f, 0.0f), param_.popInterval, (int)EasingType::InOut::Sine, LoopType::RETURN);
	} 
}
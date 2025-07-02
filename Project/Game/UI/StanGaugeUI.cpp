#include "StanGaugeUI.h"
#include "Engine.h"
#include "Engine/Lib/Json/JsonItems.h"

void StanGaugeUI::Init(const Vector2& scale, const Vector2& pos) {
	gauge_ = Engine::CreateSprite("gauge.png");
	bg_ = Engine::CreateSprite("postureStability_bg.png");

	param_.FromJson(JsonItems::GetData("UI", param_.GetName()));

	scale_ = scale;

	gauge_->SetTranslate(pos);
	gauge_->SetScale(scale);
	bg_->SetTranslate(pos);
	bg_->SetScale(scale);

	scaleTween_.Init(&scale_, (scale_ * 1.2f), scale_, 0.2f, (int)EasingType::InOut::Sine, LoopType::STOP);
	alphaTween_.Init(&alpha_, param_.minAlpha, param_.maxAlpha, param_.popInterval, (int)EasingType::InOut::Sine, LoopType::RETURN);
}

void StanGaugeUI::Update() {
	scaleTween_.Update(GameTimer::DeltaTime());
	alphaTween_.Update(GameTimer::DeltaTime());

	gauge_->SetScale(scale_);
	gauge_->SetColor(Vector4(1.0f, 0.2f, 0.2f, alpha_));

	gauge_->Update();
	bg_->Update();
}

void StanGaugeUI::Draw() const {
	Pipeline* pso = Engine::GetLastUsedPipeline();
	bg_->Draw(pso);
	gauge_->Draw(pso);
}

void StanGaugeUI::Debug_Gui() {
	gauge_->Debug_Gui();

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

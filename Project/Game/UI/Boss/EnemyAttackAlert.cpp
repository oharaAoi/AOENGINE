#include "EnemyAttackAlert.h"
#include "Engine.h"

void EnemyAttackAlert::Init() {
	alert_ = Engine::CreateSprite("attackAlert.png");
	timer_ = 0.0f;
	blinkingCount_ = 0;
	isDraw_ = true;
}

void EnemyAttackAlert::Update() {
	alert_->SetTranslate(centerPos_);
	alert_->SetScale(scale_);

	alert_->Update();
	
	Alert();
}

void EnemyAttackAlert::Draw() const {
	if (!isDraw_) { return; }
	Pipeline* pso = Engine::GetLastUsedPipeline();
	alert_->Draw(pso);
}

void EnemyAttackAlert::Debug_Gui() {
	ImGui::DragFloat2("Scale", &scale_.x, 0.1f);
	ImGui::DragFloat2("CenterPos", &centerPos_.x, 0.1f);
}

void EnemyAttackAlert::Alert() {
	timer_ += GameTimer::DeltaTime();
	if (timer_ > blinkingTime_) {
		timer_ = 0.0f;
		isDraw_ = !isDraw_;
		blinkingCount_++;
	}
}

bool EnemyAttackAlert::IsDestroy() {
	if (blinkingCount_ >= 3) {
		return true;
	}
	return false;
}

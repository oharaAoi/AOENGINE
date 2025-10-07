#include "EnemyAttackAlert.h"
#include "Engine.h"
#include "Engine/System/Audio/AudioPlayer.h"

void EnemyAttackAlert::Init() {
	Canvas2d* canvas = Engine::GetCanvas2d();
	alert_ = canvas->AddSprite("attackAlert.png", "alert");;
	timer_ = 0.0f;
	blinkingCount_ = 0;
	isDraw_ = true;

	AudioPlayer::SinglShotPlay("attackAlertSE.mp3", 0.6f);
}

void EnemyAttackAlert::Update() {
	alert_->SetTranslate(centerPos_);
	alert_->SetScale(scale_);

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

		if (isDraw_) {
			AudioPlayer::SinglShotPlay("attackAlertSE.mp3", 0.6f);
		}
	}
}

bool EnemyAttackAlert::IsDestroy() {
	if (blinkingCount_ >= 3) {
		alert_->SetIsDestroy(true);
		return true;
	}
	return false;
}

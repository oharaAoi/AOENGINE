#include "EnemyAttackAlert.h"
#include "Engine.h"
#include "Engine/System/Audio/AudioPlayer.h"
#include "Engine/Lib/GameTimer.h"

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void EnemyAttackAlert::Init(AttackAlertDirection _dir) {
	AOENGINE::Canvas2d* canvas = Engine::GetCanvas2d();
	alert_ = canvas->AddSprite("attackAlert.png", "alert");;
	timer_ = 0.0f;
	blinkingCount_ = 0;
	isDraw_ = true;

	switch (_dir) {
	case AttackAlertDirection::Front:
		alert_->SetRotate(0);
		break;
	case AttackAlertDirection::Back:
		alert_->SetRotate(180.f * kToRadian);
		break;
	case AttackAlertDirection::Left:
		alert_->SetRotate(90.f * kToRadian);
		break;
	case AttackAlertDirection::Right:
		alert_->SetRotate(-90.f * kToRadian);
		break;
	default:
		break;
	}

	AudioPlayer::SinglShotPlay("attackAlertSE.mp3", 0.6f);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void EnemyAttackAlert::Update() {
	alert_->SetTranslate(centerPos_);
	alert_->SetScale(scale_);

	Alert();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 描画処理
///////////////////////////////////////////////////////////////////////////////////////////////

void EnemyAttackAlert::Draw() const {
	if (!isDraw_) { return; }
	AOENGINE::Pipeline* pso = Engine::GetLastUsedPipeline();
	alert_->Draw(pso);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集処理
///////////////////////////////////////////////////////////////////////////////////////////////

void EnemyAttackAlert::Debug_Gui() {
	ImGui::DragFloat2("Scale", &scale_.x, 0.1f);
	ImGui::DragFloat2("CenterPos", &centerPos_.x, 0.1f);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 警告処理
///////////////////////////////////////////////////////////////////////////////////////////////

void EnemyAttackAlert::Alert() {
	timer_ += AOENGINE::GameTimer::DeltaTime();
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
	if (blinkingCount_ >= kBlinkingCount_) {
		alert_->SetIsDestroy(true);
		return true;
	}
	return false;
}

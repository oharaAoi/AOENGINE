#include "BossEvaluationFormula.h"
#include "Game/Actor/Boss/Boss.h"
#include "Engine/Lib/GameTimer.h"

void BossEvaluationFormula::Init(Boss* _pBoss) {
	pBoss_ = _pBoss;
}

void BossEvaluationFormula::Update() {
	// プレイヤーとボスの距離が一定期間近かったら
	if ((pBoss_->GetPosition() - pBoss_->GetTargetPos()).Length() < 8.0f) {
		nearTimer_ += AOENGINE::GameTimer::DeltaTime();
	} else {
		nearTimer_ = 0.0f;
	}

	// ダメージが減っている割合を算出する
	damageRate_ = pBoss_->GetParameter().health - pBoss_->GetParameter().health;
	preDamage_ = pBoss_->GetParameter().health;
}

float BossEvaluationFormula::HeightEvaluation(float min, float max) {
	// Bossの高さとPlayerの高さの差を取る
	float distance = pBoss_->GetPosition().y - pBoss_->GetTargetPos().y;
	float value = Normalize(distance, min, max);
	return std::fabs(value);
}

float BossEvaluationFormula::ApproachEvaluation(float min, float max) {
	float distance = (pBoss_->GetPosition() - pBoss_->GetTargetPos()).Length();
	float value = Normalize(distance, min, max);
	return std::fabs(value);
}

float BossEvaluationFormula::LeaveEvaluation(float min, float max) {
	float value = Normalize(nearTimer_, min, max);
	return std::fabs(value);
}

float BossEvaluationFormula::StrafeEvaluation(float min, float max) {
	float value = Normalize(damageRate_, min, max);
	return std::fabs(value);
}

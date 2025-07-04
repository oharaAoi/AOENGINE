#include "BossActionApproach.h"
#include "Engine/Lib/Math/MyMath.h"
#include "Engine/Lib/Json/JsonItems.h"
#include "Game/Actor/Boss/Boss.h"
#include "Game/Actor/Boss/Action/BossActionIdle.h"

void BossActionApproach::Debug_Gui() {
	ImGui::DragFloat("moveSpeed", &initParam_.moveSpeed, .1f);
	ImGui::DragFloat("moveTime", &initParam_.moveTime, .1f);
	ImGui::DragFloat("deceleration", &initParam_.deceleration, .1f);
	ImGui::DragFloat("maxSpinDistance", &initParam_.maxSpinDistance, .1f);
	ImGui::DragFloat("quitApproachLength", &initParam_.quitApproachLength, .1f);
	ImGui::DragFloat("decayRate", &initParam_.decayRate, 0.1f);
	
	if (ImGui::Button("Save")) {
		JsonItems::Save(pManager_->GetName(), initParam_.ToJson(initParam_.GetName()));
	}
	if (ImGui::Button("Apply")) {
		initParam_.FromJson(JsonItems::GetData(pManager_->GetName(), initParam_.GetName()));
		param_ = initParam_;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 設定時のみ行う処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossActionApproach::Build() {
	SetName("actionApprocach");
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossActionApproach::OnStart() {
	actionTimer_ = 0;

	initParam_.FromJson(JsonItems::GetData(pManager_->GetName(), initParam_.GetName()));
	param_ = initParam_;

	// player方向を計算する
	toPlayer_ = pOwner_->GetPlayerPosition() - pOwner_->GetPosition();
	distance_ = toPlayer_.Length();
	direToPlayer_ = toPlayer_.Normalize();

	lateral_ = Vector3::Cross(CVector3::UP, direToPlayer_).Normalize();

	spinAmount_ = Clamp01(distance_ / param_.maxSpinDistance);
	offsetDire_ = direToPlayer_ + lateral_ * spinAmount_;
	offsetDire_ = offsetDire_.Normalize();

	isShot_ = true;
	stopping_ = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossActionApproach::OnUpdate() {
	actionTimer_ += GameTimer::DeltaTime();
	Approach();

	if (actionTimer_ > param_.moveTime) {
		stopping_ = true;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 終了処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossActionApproach::OnEnd() {
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 次のアクションのチェック
///////////////////////////////////////////////////////////////////////////////////////////////

void BossActionApproach::CheckNextAction() {
	if (distance_ < param_.quitApproachLength) {
		size_t hash = pOwner_->GetAI()->AttackActionAI();
		NextAction(hash);
	}

	if (actionTimer_ > initParam_.moveTime) {
		size_t hash = pOwner_->GetAI()->AttackActionAI();
		NextAction(hash);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 入力処理
///////////////////////////////////////////////////////////////////////////////////////////////

bool BossActionApproach::IsInput() {
	return false;
}

void BossActionApproach::Approach() {
	pOwner_->GetTransform()->MoveVelocity(toPlayer_.Normalize() * param_.moveSpeed * GameTimer::DeltaTime(), 0.1f);
}

void BossActionApproach::SpinApproach() {
	// player方向を計算する
	toPlayer_ = pOwner_->GetPlayerPosition() - pOwner_->GetPosition();
	distance_ = toPlayer_.Length();
	direToPlayer_ = toPlayer_.Normalize();
	
	// Y軸を上とした場合、横方向ベクトルを計算
	lateral_ = Vector3::Cross(CVector3::UP, direToPlayer_).Normalize();

	// 旋回の強さを距離に応じて調整（例：最大1.0）
	spinAmount_ = Clamp01(distance_ / param_.maxSpinDistance);
	// 横にずらす（スピン方向はランダムまたは固定）
	offsetDire_ = direToPlayer_ + lateral_ * spinAmount_;
	offsetDire_ = offsetDire_.Normalize();

	// 移動をさせる
	pOwner_->GetTransform()->MoveVelocity(offsetDire_ * param_.moveSpeed * GameTimer::DeltaTime(), 0.1f);

	param_.moveSpeed -= param_.deceleration * GameTimer::DeltaTime();
}

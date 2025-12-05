#include "BossActionDualStageMissile.h"
#include "Game/Actor/Boss/Boss.h"
#include "Game/UI/Boss/BossUIs.h"
#include "Game/Actor/Boss/Bullet/BossMissile.h"

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossActionDualStageMissile::Debug_Gui() {
	ITaskNode::Debug_Gui();
	param_.Debug_Gui();
}

void BossActionDualStageMissile::Parameter::Debug_Gui() {
	ImGui::DragFloat("recovery", &recoveryTime, 1.0f);
	ImGui::DragFloat("bulletSpeed", &bulletSpeed, 1.0f);
	ImGui::DragFloat("takeDamage", &takeDamage, 1.0f);
	ImGui::DragScalar("fireNum", ImGuiDataType_U32, &fireNum);
	ImGui::DragFloat("firstSpeedRaito", &firstSpeedRaito, 0.1f);
	ImGui::DragFloat("trakingRaito", &trakingRaito, 0.1f);
	ImGui::DragFloat("spreadAngleDeg", &spreadAngleDeg, 0.1f);
	ImGui::DragFloat("secondPhaseTime", &secondPhaseTime, 0.1f);
	ImGui::DragFloat("lineOffset", &lineOffset, 0.1f);
	SaveAndLoad();
}


///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 実行処理
///////////////////////////////////////////////////////////////////////////////////////////////

BehaviorStatus BossActionDualStageMissile::Execute() {
    return Action();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 評価値の計算
///////////////////////////////////////////////////////////////////////////////////////////////

float BossActionDualStageMissile::EvaluateWeight() {
    return weight_;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 終了確認
///////////////////////////////////////////////////////////////////////////////////////////////

bool BossActionDualStageMissile::IsFinish() {
	if (isFinishShot_) {
		return true;
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 実行確認
///////////////////////////////////////////////////////////////////////////////////////////////

bool BossActionDualStageMissile::CanExecute() {
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossActionDualStageMissile::Init() {
	param_.SetGroupName("BossAction");
	param_.Load();

	taskTimer_ = 0.f;
	waitTimer_.targetTime_ = param_.recoveryTime;

	isSecondPhase_ = false;
	isFinishShot_ = false;

	// 警告を出す
	pTarget_->GetUIs()->PopAlert(pTarget_->GetTargetPos(), pTarget_->GetPosition());
	pTarget_->SetIsAttack(false);

	secondPhaseTimer_ = Timer(param_.secondPhaseTime);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossActionDualStageMissile::Update() {
	LookPlayer();

	if (!secondPhaseTimer_.Run(GameTimer::DeltaTime())) {
		Shot();
		secondPhaseTimer_.timer_ = 0.0f;

		isSecondPhase_ = true;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 終了処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossActionDualStageMissile::End() {
	pTarget_->SetIsAttack(true);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 弾を撃つ
///////////////////////////////////////////////////////////////////////////////////////////////

void BossActionDualStageMissile::Shot() {
	Vector3 pos = pTarget_->GetTransform()->GetPos();
	Quaternion rot = pTarget_->GetTransform()->GetRotate();

	Vector3 forward = rot.MakeForward();
	Vector3 right = rot.MakeRight();
	Vector3 up = rot.MakeUp(); // Y軸に限らず回転軸として使う
	Vector3 left = -right;

	Vector3 baseDire[3] = { right, up, left };

	// バラけさせる最大角度
	float totalAngleRad = param_.spreadAngleDeg * (kPI / 180.f);
	// 1発当たりの角度差
	float stepAngleRad = 0;
	if (param_.fireNum > 1) {
		stepAngleRad = totalAngleRad / float(param_.fireNum - 1);
	} else {
		stepAngleRad = 0.0f;
	}
	// 中心空左右対称にたい場合の基準オフセット
	float centerIndex = (param_.fireNum - 1) * 0.5f;

	for (uint32_t filreAngleCount = 0; filreAngleCount < 3; ++filreAngleCount) {
		for (uint32_t fireAngle = 0; fireAngle < param_.fireNum; ++fireAngle) {
			// i番目の弾の角度オフセット
			float angleRad = (float(fireAngle) - centerIndex) * stepAngleRad;

			// up軸周りにangleRad回転するクォータニオンを作成
			Quaternion q = Quaternion::AngleAxis(angleRad, up);

			// 基準方向を回転させてdirectionを作る
			Vector3 dire = q * baseDire[filreAngleCount];
			dire = dire.Normalize();

			for (int line = -1; line <= 1; line += 2) {
				// -1 = 左列,  +1 = 右列
				Vector3 col = up;
				if (filreAngleCount == 1) {
					col = right;
				}

				Vector3 spawnPos = pos + col * (line * param_.lineOffset);

				Vector3 velocity = dire.Normalize() * param_.bulletSpeed;
				BossMissile* missile = pTarget_->GetBulletManager()->AddBullet<BossMissile>(spawnPos, velocity, pTarget_->GetTargetPos(),
																							param_.bulletSpeed, param_.firstSpeedRaito, param_.trakingRaito, true);
				missile->SetTakeDamage(param_.takeDamage);

			}
		}
	}

	// 終了確認
	if (isSecondPhase_) {
		isFinishShot_ = true;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ playerの方向を向く
///////////////////////////////////////////////////////////////////////////////////////////////

void BossActionDualStageMissile::LookPlayer() {
	Quaternion targetToRotation_ = Quaternion::LookAt(pTarget_->GetPosition(), pTarget_->GetTargetPos());
	Quaternion lookRotation = Quaternion::Slerp(pTarget_->GetTransform()->GetRotate(), targetToRotation_);
	pTarget_->GetTransform()->SetRotate(lookRotation);
}
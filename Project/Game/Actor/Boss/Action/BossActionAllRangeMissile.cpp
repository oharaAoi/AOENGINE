#include "BossActionAllRangeMissile.h"
#include "Game/Actor/Boss/Boss.h"
#include "Game/Actor/Boss/Bullet/BossMissile.h"
#include "Game/Actor/Boss/Action/BossActionIdle.h"

void BossActionAllRangeMissile::Debug_Gui() {

}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 設定時のみ行う処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossActionAllRangeMissile::Build() {
	SetName("allRangeMissile");
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 開始処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossActionAllRangeMissile::OnStart() {
	actionTimer_ = 0.f;
	playerToRotation_ = Quaternion::LookAt(pOwner_->GetPosition(), pOwner_->GetPlayerPosition());

	mainAction_ = std::bind(&BossActionAllRangeMissile::LookPlayer, this);

	isFinishShot_ = false;

	bulletSpeed_ = 60.f;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossActionAllRangeMissile::OnUpdate() {
	actionTimer_ += GameTimer::DeltaTime();

	mainAction_();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 終了処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossActionAllRangeMissile::OnEnd() {
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 次のアクションのチェック
///////////////////////////////////////////////////////////////////////////////////////////////

void BossActionAllRangeMissile::CheckNextAction() {
	if (isFinishShot_) {
		NextAction<BossActionIdle>();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 入力処理
///////////////////////////////////////////////////////////////////////////////////////////////

bool BossActionAllRangeMissile::IsInput() {
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 弾を撃つ処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossActionAllRangeMissile::Shot() {
	if (isFinishShot_) { return; }
	Vector3 pos = pOwner_->GetTransform()->translate_;
	Vector3 forward = pOwner_->GetTransform()->rotation_.MakeForward();
	Vector3 right = pOwner_->GetTransform()->rotation_.MakeRight();
	Vector3 up = pOwner_->GetTransform()->rotation_.MakeUp(); // Y軸に限らず回転軸として使う

	const uint32_t angleUpNum = 4;
	const uint32_t fireNum = 9;
	const float halfAngle = kPI * 0.5f; // 90度

	for (uint32_t upCount = 0; upCount < angleUpNum; ++upCount) {
		for (uint32_t i = 0; i < fireNum; ++i) {
			// -90度〜+90度に等間隔で弾を発射
			float angle = -halfAngle + (kPI * i) / (fireNum - 1);
			float upAngle = -halfAngle * ((float)upCount / (float)angleUpNum);

			// クォータニオンで回転を生成（up軸周りに角度回転）
			Quaternion rot = Quaternion::AngleAxis(angle, up);
			Quaternion upoRot = Quaternion::AngleAxis(upAngle, right);

			// 正面ベクトルを回転させて発射方向に
			Vector3 dir = upoRot.Rotate(forward);
			dir = rot.Rotate(dir);

			Vector3 velocity = dir.Normalize() * bulletSpeed_;
			pOwner_->GetBulletManager()->AddBullet<BossMissile>(pos, velocity, pOwner_->GetPlayerPosition(), bulletSpeed_, 0.05f, true);
		}
	}

	isFinishShot_ = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ Playerの方向を向かせる処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossActionAllRangeMissile::LookPlayer() {
	float t = actionTimer_ / lookTime_;
	Quaternion lookRotation = Quaternion::Slerp(pOwner_->GetTransform()->GetQuaternion(), playerToRotation_, t);
	pOwner_->GetTransform()->SetQuaternion(lookRotation);

	// 次の行動に遷移する
	if (actionTimer_ > lookTime_) {
		mainAction_ = std::bind(&BossActionAllRangeMissile::Shot, this);
	}
}

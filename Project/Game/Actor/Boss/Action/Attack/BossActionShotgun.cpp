#include "BossActionShotgun.h"
#include "Game/Actor/Boss/Boss.h"
#include "Game/Actor/Boss/Bullet/BossBullet.h"
#include "Game/Actor/Boss/Action/BossActionIdle.h"
#include "Engine/Lib/Math/MyRandom.h"
#include "Engine/Lib/Json/JsonItems.h"
#include "Game/UI/Boss/BossUIs.h"

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossActionShotgun::Debug_Gui() {
	if (ImGui::CollapsingHeader("Parameter")) {
		ImGui::DragFloat("bulletSpeed", &param_.bulletSpeed, .1f);
		ImGui::DragFloat("stiffenTime", &param_.bulletSpread, .1f);
		ImGui::DragInt("kFireCount", &param_.kFireCount, 1);

		if (ImGui::Button("Save")) {
			JsonItems::Save(pManager_->GetName(), param_.ToJson(param_.GetName()));
		}
		if (ImGui::Button("Apply")) {
			param_.FromJson(JsonItems::GetData(pManager_->GetName(), param_.GetName()));
		}
	}

	weight_->Debug_Gui();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 設定処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossActionShotgun::Build() {
	SetName("actionShotgun");
	param_.FromJson(JsonItems::GetData(pManager_->GetName(), param_.GetName()));

	weight_ = std::make_unique<BossLotteryAction>();
	weight_->Init("actionShotgunWeight");

	size_t hash = typeid(BossActionShotgun).hash_code();
	pOwner_->GetAI()->SetAttackWeight(hash, weight_.get());
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossActionShotgun::OnStart() {
	actionTimer_ = 0.0f;
	isFinishShot_ = false;
	Shot();

	// 警告を出す
	pOwner_->GetUIs()->PopAlert();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossActionShotgun::OnUpdate() {
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 終了処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossActionShotgun::OnEnd() {
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 次のアクションへの遷移確認
///////////////////////////////////////////////////////////////////////////////////////////////

void BossActionShotgun::CheckNextAction() {
	if (isFinishShot_) {
		NextAction<BossActionIdle>();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 入力処理
///////////////////////////////////////////////////////////////////////////////////////////////

bool BossActionShotgun::IsInput() {
	return false;
}

void BossActionShotgun::Shot() {
	Vector3 pos = pOwner_->GetPosition();
	Vector3 velocity = (pOwner_->GetPlayerPosition() - pos).Normalize();
	// ばらつきを弧度法に
	float bulletSpread = param_.bulletSpread * kToRadian;

	// 弾数分処理をする
	for (int oi = 0; oi < param_.kFireCount; ++oi) {
		float yawOffset = RandomFloat(-bulletSpread, bulletSpread);
		float pitchOffset = RandomFloat(-bulletSpread, bulletSpread);

		Quaternion yawRot = Quaternion::AngleAxis(yawOffset, CVector3::UP);
		Quaternion pitchRot = Quaternion::AngleAxis(pitchOffset, CVector3::RIGHT);
		Quaternion spreadRot = yawRot * pitchRot;

		Vector3 dir = spreadRot * velocity;
		BossBullet* bullet = pOwner_->GetBulletManager()->AddBullet<BossBullet>(pos, dir * param_.bulletSpeed);
		bullet->SetTakeDamage(10.0f);
	}

	isFinishShot_ = true;
}
#include "BaseWeapon.h"
#include "Engine/Render/SceneRenderer.h"
#include "Engine/Lib/Json/JsonItems.h"

void BaseWeapon::Finalize() {
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BaseWeapon::Debug_Gui() {
	transform_->Debug_Gui();
	ImGui::SliderFloat("coolTime", &coolTime_, 0.0f, attackParam_.fireInterval);
	ImGui::Text("fireCount : %d", fireCount_);
	attackParam_.Debug_Gui();
}

void BaseWeapon::AttackParam::Debug_Gui() {
	ImGui::DragFloat("bulletSpeed", &bulletSpeed, 0.1f);
	ImGui::DragInt("maxBulletsNum", &maxBulletsNum);
	ImGui::DragInt("fireBulletsNum", &fireBulletsNum);
	ImGui::DragFloat("fireInterval", &fireInterval, 0.1f);
	ImGui::DragFloat("reloadTime", &reloadTime, 0.1f);
	ImGui::DragFloat("takeDamage", &takeDamage, 0.1f);
	ImGui::DragFloat("startUpTime", &startUpTime, 0.1f);
	ImGui::DragFloat("recoilTime", &recoilTime, 0.1f);
	SaveAndLoad();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BaseWeapon::Init() {
	object_ = SceneRenderer::GetInstance()->AddObject<BaseGameObject>("Weapon", "Object_Normal.json");
	transform_ = object_->GetTransform();
	isCanAttack_ = true;
	isReload_ = false;
	coolTime_ = 0.0f;
	fireCount_ = 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BaseWeapon::Update() {
	transform_->Update();
	if (coolTime_ >= 0.0f) {
		coolTime_ -= AOENGINE::GameTimer::DeltaTime();
	} else {
		isReload_ = false;
		isCanAttack_ = true;
		coolTime_ = 0;

		// 発射できる数のリセット
		if (fireCount_ >= attackParam_.fireBulletsNum) {
			attackParam_.maxBulletsNum -= attackParam_.fireBulletsNum;
			fireCount_ = 0;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 攻撃後の処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BaseWeapon::AttackAfter() {
	isCanAttack_ = false;	// 攻撃のcoolTimeフラグ
	fireCount_++;			// 現在の発射数

	// マガジンのの上限を超えたらリロードのタイムをcoolTimeに
	if (fireCount_ >= attackParam_.fireBulletsNum) {
		coolTime_ = attackParam_.reloadTime;
		isReload_ = true;
	} else {
		coolTime_ = attackParam_.fireInterval;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ バレットの数などの割合を出す
///////////////////////////////////////////////////////////////////////////////////////////////

float BaseWeapon::BulletsFill() {
	return static_cast<float>(attackParam_.fireBulletsNum - fireCount_) / static_cast<float>(attackParam_.fireBulletsNum);
}

float BaseWeapon::ReloadFill() {
	return (attackParam_.reloadTime - coolTime_) / attackParam_.reloadTime;
}
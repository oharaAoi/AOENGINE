#include "BaseWeapon.h"
#include "Engine/Render/SceneRenderer.h"
#include "Engine/Lib/Json/JsonItems.h"

void BaseWeapon::Finalize() {
}

void BaseWeapon::Init() {
	object_ = SceneRenderer::GetInstance()->AddObject<BaseGameObject>("Weapon", "Object_Normal.json");
	transform_ = object_->GetTransform();
	isCanAttack_ = true;
	coolTime_ = 0.0f;
	fireCount_ = 0;
}

void BaseWeapon::Update() {
	if (coolTime_ >= 0.0f) {
		coolTime_ -= GameTimer::DeltaTime();
	} else {
		isCanAttack_ = true;
		coolTime_ = 0;

		// 発射できる数のリセット
		if (fireCount_ >= attackParam_.fireBulletsNum) {
			attackParam_.maxBulletsNum -= attackParam_.fireBulletsNum;
			fireCount_ = 0;
		}
	}
}

void BaseWeapon::Debug_Gui() {
	transform_->Debug_Gui();
	ImGui::SliderFloat("coolTime", &coolTime_, 0.0f, attackParam_.fireInterval);
	attackParam_.Debug_Gui();
}

void BaseWeapon::AttackAfter() {
	isCanAttack_ = false;	// 攻撃のcoolTimeフラグ
	fireCount_++;			// 現在の発射数

	// マガジンのの上限を超えたらリロードのタイムをcoolTimeに
	if (fireCount_ >= attackParam_.fireBulletsNum) {
		coolTime_ = attackParam_.reloadTime;
	} else {
		coolTime_ = attackParam_.fireInterval;
	}
}

float BaseWeapon::BulletsFill() {
	return static_cast<float>(attackParam_.fireBulletsNum - fireCount_) / static_cast<float>(attackParam_.fireBulletsNum);
}

void BaseWeapon::AttackParam::Debug_Gui() {
	ImGui::DragFloat("bulletSpeed", &bulletSpeed, 0.1f);
	ImGui::DragInt("maxBulletsNum", &maxBulletsNum);
	ImGui::DragInt("fireBulletsNum", &fireBulletsNum);
	ImGui::DragFloat("fireInterval", &fireInterval, 0.1f);
	ImGui::DragFloat("reloadTime", &reloadTime, 0.1f);
	ImGui::DragFloat("takeDamage", &takeDamage, 0.1f);
	SaveAndLoad();
}

#include "ShoulderMissile.h"
#include "Engine/Lib/Json/JsonItems.h"
#include "Game/Actor/Weapon/Bullet/RocketBullet.h"

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 終了処理
///////////////////////////////////////////////////////////////////////////////////////////////

void ShoulderMissile::Finalize() {

}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void ShoulderMissile::Init() {
	SetName("ShoulderMissile");
	BaseWeapon::Init();
	attackParam_.SetName("ShoulderMissileAttackParam");
	attackParam_.FromJson(JsonItems::GetData(GetName(), attackParam_.GetName()));
	weaponParam_.FromJson(JsonItems::GetData(GetName(), weaponParam_.GetName()));

	object_->SetObject("shoulderMissile.obj");

	transform_->SetTranslate(weaponParam_.pos);

	EditorWindows::AddObjectWindow(this, GetName());
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集処理
///////////////////////////////////////////////////////////////////////////////////////////////

void ShoulderMissile::Debug_Gui() {
	BaseWeapon::Debug_Gui();
	ImGui::DragFloat3("pos", &weaponParam_.pos.x, 0.1f);

	if (ImGui::Button("Save")) {
		JsonItems::Save(GetName(), weaponParam_.ToJson(weaponParam_.GetName()));
	}
	transform_->SetTranslate(weaponParam_.pos);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 攻撃処理
///////////////////////////////////////////////////////////////////////////////////////////////

void ShoulderMissile::Attack(const AttackContext& cxt) {
	Vector3 worldPos = object_->GetPosition();
	RocketBullet* bullet = pBulletManager_->AddBullet<RocketBullet>(worldPos, cxt.target, attackParam_.bulletSpeed);
	bullet->SetTakeDamage(20.0f);
}

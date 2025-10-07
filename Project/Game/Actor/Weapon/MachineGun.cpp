#include "MachineGun.h"
#include "Engine/System/Manager/ParticleManager.h"
#include "Engine/Lib/Json/JsonItems.h"

void MachineGun::Finalize() {
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集
///////////////////////////////////////////////////////////////////////////////////////////////

void MachineGun::Debug_Gui() {
	BaseWeapon::Debug_Gui();
	ImGui::DragFloat3("offset", &offset_.x, .1f);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化
///////////////////////////////////////////////////////////////////////////////////////////////

void MachineGun::Init() {
	SetName("MachineGun");
	BaseWeapon::Init();
	attackParam_.SetName("MachineGunAttackParam");
	attackParam_.Load();
	
	object_->SetObject("gun.obj");
	transform_->srt_.rotate = Quaternion::AngleAxis(kPI, CVector3::FORWARD) * Quaternion::AngleAxis(kHPI, CVector3::RIGHT);

	// -------------------------------------------------
	// ↓ Effect関連
	// -------------------------------------------------
	gunFireParticles_ = ParticleManager::GetInstance()->CrateParticle("gunFireParticles");
	gunFireParticles_->SetParent(transform_->GetWorldMatrix());

	EditorWindows::AddObjectWindow(this, GetName());
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 弾を撃つ
///////////////////////////////////////////////////////////////////////////////////////////////

bool MachineGun::Attack(const AttackContext& cxt) {
	if (!isCanAttack_) { return false; }

	Vector3 worldPos = object_->GetPosition();
	PlayerBullet* bullet = pBulletManager_->AddBullet<PlayerBullet>(worldPos, cxt.direction * attackParam_.bulletSpeed);
	bullet->SetTakeDamage(attackParam_.takeDamage);
	// effectを出す
	Vector3 pos = offset_;
	pos = (cxt.direction * offset_.z);
	gunFireParticles_->SetPos(pos);
	gunFireParticles_->SetIsStop(false);

	AttackAfter();
	return true;
}
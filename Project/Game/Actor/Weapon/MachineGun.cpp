#include "MachineGun.h"
#include "Engine/System/Manager/ParticleManager.h"

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
	BaseWeapon::Init();
	
	object_->SetObject("gun.obj");
	SetName("MachineGun");
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

void MachineGun::Attack(const AttackContext& cxt) {
	Vector3 worldPos = object_->GetPosition();
	PlayerBullet* bullet = pBulletManager_->AddBullet<PlayerBullet>(worldPos, cxt.direction * speed_);
	bullet->SetTakeDamage(10.0f);
	// effectを出す
	Vector3 pos = offset_;
	pos = (cxt.direction * offset_.z);
	gunFireParticles_->SetPos(pos);
	gunFireParticles_->SetIsStop(false);
}
#include "MachineGun.h"
#include "Engine/System/Manager/ParticleManager.h"

void MachineGun::Finalize() {
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集
///////////////////////////////////////////////////////////////////////////////////////////////

void MachineGun::Debug_Gui() {
	transform_->Debug_Gui();

	BaseWeapon::Debug_Gui();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化
///////////////////////////////////////////////////////////////////////////////////////////////

void MachineGun::Init() {
	BaseWeapon::Init();
	
	object_->SetObject("gun.obj");
	SetName("MachineGun");

	transform_->translate_ = { 1.2f, 1.1f, 0.0f };

	// -------------------------------------------------
	// ↓ Effect関連
	// -------------------------------------------------
	gunFireParticles_ = ParticleManager::GetInstance()->CrateParticle("gunFireParticles");

	EditorWindows::AddObjectWindow(this, GetName());
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新
///////////////////////////////////////////////////////////////////////////////////////////////

void MachineGun::Update() {
	BaseWeapon::Update();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 描画
///////////////////////////////////////////////////////////////////////////////////////////////

void MachineGun::Draw() const {
	BaseWeapon::Draw();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 弾を撃つ
///////////////////////////////////////////////////////////////////////////////////////////////

void MachineGun::Shot(const Vector3& direction, uint32_t type) {
	Vector3 worldPos = object_->GetPosition();
	PlayerBullet* bullet = pBulletManager_->AddBullet(worldPos, direction * speed_, type);
	bullet->SetTakeDamage(10.0f);
	// effectを出す
	Vector3 pos = worldPos;
	pos += (direction * 4.f);
	gunFireParticles_->SetPos(pos);
	gunFireParticles_->SetIsStop(false);
}
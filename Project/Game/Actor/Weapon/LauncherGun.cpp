#include "LauncherGun.h"
#include "Engine/System/Manager/ParticleManager.h"
#include "Engine/Lib/Json/JsonItems.h"

void LauncherGun::Finalize() {
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集
///////////////////////////////////////////////////////////////////////////////////////////////

void LauncherGun::Debug_Gui() {
	BaseWeapon::Debug_Gui();
	ImGui::DragFloat3("offset", &offset_.x, .1f);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化
///////////////////////////////////////////////////////////////////////////////////////////////

void LauncherGun::Init() {
	SetName("LauncherGun");
	BaseWeapon::Init();
	attackParam_.SetName("LauncherAttackParam");
	attackParam_.Load();

	object_->SetObject("launcher.obj");
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
 
void LauncherGun::Attack(const AttackContext& cxt) {
	if (!isCanAttack_) { return; }

	object_->Update();

	Vector3 worldPos = object_->GetPosition();
	PlayerBullet* bullet = pBulletManager_->AddBullet<PlayerBullet>(worldPos, cxt.direction * attackParam_.bulletSpeed);
	bullet->SetTakeDamage(20.0f);
	// effectを出す
	Vector3 pos = offset_;
	pos = (cxt.direction * offset_.z);
	gunFireParticles_->SetPos(pos);
	gunFireParticles_->SetIsStop(false);

	AttackAfter();
}
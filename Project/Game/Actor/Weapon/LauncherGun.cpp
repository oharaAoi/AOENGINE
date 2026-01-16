#include "LauncherGun.h"
#include "Engine/System/Manager/ParticleManager.h"
#include "Engine/System/Editor/Window/EditorWindows.h"
#include "Engine/System/Audio/AudioPlayer.h"
#include "Game/Actor/Weapon/Bullet/LauncherBullet.h"

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

	object_->SetObject("launcher.obj", MaterialType::PBR);
	transform_->srt_.rotate = Math::Quaternion::AngleAxis(kPI, CVector3::FORWARD) * Math::Quaternion::AngleAxis(kHPI, CVector3::RIGHT);

	// -------------------------------------------------
	// ↓ Effect関連
	// -------------------------------------------------

	gunFireParticles_ = AOENGINE::ParticleManager::GetInstance()->CrateParticle("gunFireParticles");
	gunFireParticles_->SetParent(transform_->GetWorldMatrix());

	AOENGINE::EditorWindows::AddObjectWindow(this, GetName());
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 弾を撃つ
///////////////////////////////////////////////////////////////////////////////////////////////
 
bool LauncherGun::Attack(const AttackContext& cxt) {
	if (!isCanAttack_) { return false; }

	object_->Update();

	Math::Vector3 worldPos = object_->GetPosition();
	LauncherBullet* bullet = pBulletManager_->AddBullet<LauncherBullet>(worldPos, cxt.direction * attackParam_.bulletSpeed);
	bullet->SetTakeDamage(attackParam_.takeDamage);
	// effectを出す
	Math::Vector3 pos = offset_;
	pos = (cxt.direction * offset_.z);
	gunFireParticles_->SetPos(pos);
	gunFireParticles_->SetIsStop(false);

	AOENGINE::AudioPlayer::SinglShotPlay("shotLauncher.mp3", 0.08f);

	AttackAfter();
	return true;
}
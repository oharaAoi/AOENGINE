#include "MachineGun.h"
#include "Engine/System/Manager/ParticleManager.h"
#include "Engine/System/Audio/AudioPlayer.h"
#include "Engine/System/Editor/Window/EditorWindows.h"
#include "Game/Actor/Player/Bullet/PlayerBullet.h"

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
	
	object_->SetObject("gun.obj", MaterialType::PBR);
	transform_->srt_.rotate = Math::Quaternion::AngleAxis(kPI, CVector3::FORWARD) * Math::Quaternion::AngleAxis(kHPI, CVector3::RIGHT);

	// -------------------------------------------------
	// ↓ Effect関連
	// -------------------------------------------------
	gunFireParticles_ = AOENGINE::ParticleManager::GetInstance()->CreateParticle("gunFireParticles");
	gunFireParticles_->SetParent(transform_->GetWorldMatrix());

}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 弾を撃つ
///////////////////////////////////////////////////////////////////////////////////////////////

bool MachineGun::Attack(const AttackContext& cxt) {
	if (!isCanAttack_) { return false; }

	Math::Vector3 worldPos = object_->GetPosition();
	PlayerBullet* bullet = pBulletManager_->AddBullet<PlayerBullet>(worldPos, cxt.direction * attackParam_.bulletSpeed);
	bullet->SetTakeDamage(attackParam_.takeDamage);

	// effectを出す
	Math::Vector3 pos = offset_;
	pos = (cxt.direction * offset_.z);
	gunFireParticles_->SetPos(pos);
	gunFireParticles_->SetIsStop(false);

	AOENGINE::AudioPlayer::SinglShotPlay("shotBullet.mp3", 0.06f);

	AttackAfter();
	return true;
}
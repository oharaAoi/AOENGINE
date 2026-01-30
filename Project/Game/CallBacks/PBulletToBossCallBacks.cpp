#include "PBulletToBossCallBacks.h"
#include "Engine/Render/SceneRenderer.h"
#include "Engine/System/Audio/AudioPlayer.h"
#include "Engine/System/Manager/ParticleManager.h"
#include "Game/Information/ColliderCategory.h"

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void PBulletToBossCallBacks::Init() {
	SetCallBacks();
	SetPair(pCollisionManager_, ColliderTags::Bullet::machinegun, ColliderTags::Boss::own);

	AOENGINE::ParticleManager* manager = AOENGINE::ParticleManager::GetInstance();
	hitBossSmoke_ = manager->CreateParticle("MachineGunHit");
	hitBossSmokeBorn_ = manager->CreateParticle("MachineGunHit2");
	hitSmoke_ = manager->CreateParticle("Expload");
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void PBulletToBossCallBacks::Update() {
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ コリジョン処理
///////////////////////////////////////////////////////////////////////////////////////////////

void PBulletToBossCallBacks::CollisionEnter([[maybe_unused]] AOENGINE::BaseCollider* const bullet, [[maybe_unused]] AOENGINE::BaseCollider* const boss) {
	BaseBullet* playerBullet = pBulletManager_->SearchCollider(bullet);
	// bulletの処理
	if (playerBullet != nullptr) {
		if (playerBullet->GetBulletType() == BulletType::Launcher) {
			hitSmoke_->SetPos(bullet->GetCenterPos());
			hitSmoke_->Reset();
		}

		if (playerBullet->GetBulletType() != BulletType::Laser) {
			playerBullet->SetIsAlive(false);
		}

		// bossへの処理
		pBoss_->Damage(playerBullet->GetTakeDamage());
	}

	hitBossSmoke_->SetPos(bullet->GetCenterPos());
	hitBossSmokeBorn_->SetPos(bullet->GetCenterPos());

	hitBossSmoke_->SetIsStop(false);
	hitBossSmokeBorn_->SetIsStop(false);

	AOENGINE::AudioPlayer::SinglShotPlay("bulletHit.mp3", 0.04f);
}

void PBulletToBossCallBacks::CollisionStay([[maybe_unused]] AOENGINE::BaseCollider* const bullet, [[maybe_unused]] AOENGINE::BaseCollider* const boss) {
}

void PBulletToBossCallBacks::CollisionExit([[maybe_unused]] AOENGINE::BaseCollider* const bullet, [[maybe_unused]] AOENGINE::BaseCollider* const boss) {
}

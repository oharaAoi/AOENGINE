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

	ParticleManager* manager = ParticleManager::GetInstance();
	//hitBossExploadParticles_ = manager->CrateParticle("HitBossExploadParticles");
	hitBossSmoke_ = manager->CrateParticle("MachineGunHit");
	hitBossSmokeBorn_ = manager->CrateParticle("MachineGunHit2");
	hitSmoke_ = manager->CrateParticle("HitSmoke");
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void PBulletToBossCallBacks::Update() {

	for (auto it = hitExplodeList_.begin(); it != hitExplodeList_.end(); ) {
		if (!(*it)->GetIsAlive()) {
			(*it)->SetIsDestroy(true);
			it = hitExplodeList_.erase(it); // 要素の削除とイテレータ更新
		} else {
			++it;
		}
	}

	for (auto& explode : hitExplodeList_) {
		explode->Update();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ コリジョン処理
///////////////////////////////////////////////////////////////////////////////////////////////

void PBulletToBossCallBacks::CollisionEnter([[maybe_unused]] ICollider* const bullet, [[maybe_unused]] ICollider* const boss) {
	BaseBullet* playerBullet = pBulletManager_->SearchCollider(bullet);
	// bulletの処理
	if (playerBullet != nullptr) {
		playerBullet->SetIsAlive(false);

		if (playerBullet->GetBulletType() == BulletType::Launcher) {
			auto& newExplodeBurn = hitExplodeList_.emplace_back(SceneRenderer::GetInstance()->AddObject<HitExplode>("hitExplodeBurn", "Object_Normal.json"));
			newExplodeBurn->SetBlendMode(3);
			newExplodeBurn->Init();
			newExplodeBurn->Set(bullet->GetCenterPos(), Color::RgbTo01(255.0f / 255.0f, 69.0f / 256.0f, 0), "image.png");

			auto& newExplode = hitExplodeList_.emplace_back(SceneRenderer::GetInstance()->AddObject<HitExplode>("hitExplode", "Object_Normal.json"));
			newExplode->SetBlendMode(3);
			newExplode->Init();
			newExplode->Set(bullet->GetCenterPos(), Color::RgbTo01(255.0f / 255.0f, 69.0f / 256.0f, 0), "explode.png");

			hitSmoke_->SetPos(bullet->GetCenterPos());
			hitSmoke_->Reset();
		}

		// bossへの処理
		pBoss_->Damage(playerBullet->GetTakeDamage());
	}

	hitBossSmoke_->SetPos(bullet->GetCenterPos());
	hitBossSmokeBorn_->SetPos(bullet->GetCenterPos());

	hitBossSmoke_->SetIsStop(false);
	hitBossSmokeBorn_->SetIsStop(false);

	AudioPlayer::SinglShotPlay("bulletHit.mp3", 0.04f);
}

void PBulletToBossCallBacks::CollisionStay([[maybe_unused]] ICollider* const bullet, [[maybe_unused]] ICollider* const boss) {
}

void PBulletToBossCallBacks::CollisionExit([[maybe_unused]] ICollider* const bullet, [[maybe_unused]] ICollider* const boss) {
}

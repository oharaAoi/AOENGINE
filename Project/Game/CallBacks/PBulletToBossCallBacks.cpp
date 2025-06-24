#include "PBulletToBossCallBacks.h"
#include "Engine/Render.h"
#include "Game/Information/ColliderCategory.h"
#include "Engine/System/Manager/ParticleManager.h"

void PBulletToBossCallBacks::Init() {
	SetCallBacks();
	SetPair(pCollisionManager_, ColliderTags::Bullet::machinegun, ColliderTags::Boss::own);

	ParticleManager* manager = ParticleManager::GetInstance();
	//hitBossExploadParticles_ = manager->CrateParticle("HitBossExploadParticles");
	hitBossSmoke_ = manager->CrateParticle("MachineGunHit");
	hitBossSmokeBorn_ = manager->CrateParticle("MachineGunHit2");
	hitSmoke_ = manager->CrateParticle("HitSmoke");
}

void PBulletToBossCallBacks::Update() {

	for (auto it = hitExplodeList_.begin(); it != hitExplodeList_.end(); ) {
		if (!it->get()->GetIsAlive()) {
			it = hitExplodeList_.erase(it); // 要素の削除とイテレータ更新
		} else {
			++it;
		}
	}

	for (auto& explode : hitExplodeList_) {
		explode->Update();
	}
}

void PBulletToBossCallBacks::Draw() const {
	for (const auto& explode : hitExplodeList_) {
		explode->Draw();
	}
}

void PBulletToBossCallBacks::CollisionEnter([[maybe_unused]] ICollider* const bullet, [[maybe_unused]] ICollider* const boss) {
	PlayerBullet* playerBullet = pBulletManager_->SearchCollider(bullet);
	// bulletの処理
	if (playerBullet != nullptr) {
		playerBullet->SetIsAlive(false);

		if (playerBullet->GetType() == 1) {
			auto& newExplodeBurn = hitExplodeList_.emplace_back(std::make_unique<HitExplode>());
			newExplodeBurn->SetBlendMode(3);
			newExplodeBurn->Init();
			newExplodeBurn->Set(bullet->GetCenterPos(), Vector4(255.0f / 255.0f, 69.0f / 256.0f, 0, 1.0f), "image.png");

			auto& newExplode = hitExplodeList_.emplace_back(std::make_unique<HitExplode>());
			newExplode->SetBlendMode(3);
			newExplode->Init();
			newExplode->Set(bullet->GetCenterPos(), Vector4(255.0f / 255.0f, 69.0f / 256.0f, 0, 1.0f), "explode.png");

			hitSmoke_->SetPos(bullet->GetCenterPos());
			hitSmoke_->Reset();
		}
	}

	//hitBossExploadParticles_->SetPos(bullet->GetCenterPos());
	hitBossSmoke_->SetPos(bullet->GetCenterPos());
	hitBossSmokeBorn_->SetPos(bullet->GetCenterPos());

	//hitBossExploadParticles_->SetOnShot(true);
	hitBossSmoke_->SetIsStop(false);
	hitBossSmokeBorn_->SetIsStop(false);

	// bossへの処理
	pBoss_->Damage(playerBullet->GetTakeDamage());
}

void PBulletToBossCallBacks::CollisionStay([[maybe_unused]] ICollider* const bullet, [[maybe_unused]] ICollider* const boss) {
}

void PBulletToBossCallBacks::CollisionExit([[maybe_unused]] ICollider* const bullet, [[maybe_unused]] ICollider* const boss) {
}

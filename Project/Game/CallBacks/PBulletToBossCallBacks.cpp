#include "PBulletToBossCallBacks.h"
#include "Engine/Render.h"
#include "Game/Information/ColliderCategory.h"
#include "Engine/System/Manager/ParticleManager.h"

void PBulletToBossCallBacks::Init() {
	SetCallBacks();
	SetPair(pCollisionManager_, ColliderTags::Bullet::machinegun, ColliderTags::Boss::own);

	ParticleManager* manager = ParticleManager::GetInstance();
	hitBossExploadParticles_ = manager->CrateParticle("HitBossExploadParticles");
	hitBossSmoke_ = manager->CrateParticle("HitBossSmoke");
	hitBossSmokeBorn_ = manager->CrateParticle("HitBossSmokeBorn");
}

void PBulletToBossCallBacks::Update() {
	hitBossSmoke_->Update();
	hitBossSmokeBorn_->Update();
	hitBossExploadParticles_->Update();
}

void PBulletToBossCallBacks::CollisionEnter([[maybe_unused]] ICollider* const bullet, [[maybe_unused]] ICollider* const boss) {
	PlayerBullet* playerBullet = pBulletManager_->SearchCollider(bullet);
	if (playerBullet != nullptr) {
		playerBullet->SetIsAlive(false);
	}
	
	hitBossExploadParticles_->SetPos(bullet->GetCenterPos());
	hitBossSmoke_->SetPos(bullet->GetCenterPos());
	hitBossSmokeBorn_->SetPos(bullet->GetCenterPos());

	hitBossExploadParticles_->SetOnShot(true);
	hitBossSmoke_->SetOnShot(true);
	hitBossSmokeBorn_->SetOnShot(true);
}

void PBulletToBossCallBacks::CollisionStay([[maybe_unused]] ICollider* const bullet, [[maybe_unused]] ICollider* const boss) {
}

void PBulletToBossCallBacks::CollisionExit([[maybe_unused]] ICollider* const bullet, [[maybe_unused]] ICollider* const boss) {
}

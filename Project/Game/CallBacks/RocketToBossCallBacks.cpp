#include "RocketToBossCallBacks.h"
#include "Game/Information/ColliderCategory.h"

void RocketToBossCallBacks::Init() {
	SetCallBacks();
	SetPair(pCollisionManager_, ColliderTags::Bullet::rocket, ColliderTags::Boss::own);
}

void RocketToBossCallBacks::Update() {
}

void RocketToBossCallBacks::CollisionEnter([[maybe_unused]] ICollider* const bullet, [[maybe_unused]] ICollider* const boss) {
	BaseBullet* playerBullet = pBulletManager_->SearchCollider(bullet);
	// bulletの処理
	if (playerBullet != nullptr) {
		playerBullet->SetIsAlive(false);
		// bossへの処理
		pBoss_->Damage(playerBullet->GetTakeDamage());
	}
}

void RocketToBossCallBacks::CollisionStay([[maybe_unused]] ICollider* const bullet, [[maybe_unused]] ICollider* const boss) {
}

void RocketToBossCallBacks::CollisionExit([[maybe_unused]] ICollider* const bullet, [[maybe_unused]] ICollider* const boss) {
}

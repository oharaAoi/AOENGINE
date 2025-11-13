#include "BBulletToPlayerCallBacks.h"
#include "Game/Information/ColliderCategory.h"

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BBulletToPlayerCallBacks::Init() {
	SetCallBacks();
	SetPair(pCollisionManager_, ColliderTags::Boss::missile, ColliderTags::Player::own);

	ParticleManager* manager = ParticleManager::GetInstance();
	hitEffect_ = manager->CrateParticle("MissileHit");
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BBulletToPlayerCallBacks::Update() {
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ コリジョン処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BBulletToPlayerCallBacks::CollisionEnter([[maybe_unused]] ICollider* const bullet, [[maybe_unused]] ICollider* const player) {
	BaseBullet* hitBullet = pBossBulletManager_->SearchCollider(bullet);
	if (hitBullet != nullptr) {
		hitBullet->SetIsAlive(false);
		hitEffect_->SetPos(hitBullet->GetPosition());
		hitEffect_->Reset();
	}

	pPlayer_->Damage(hitBullet->GetTakeDamage());
}

void BBulletToPlayerCallBacks::CollisionStay([[maybe_unused]] ICollider* const bullet, [[maybe_unused]] ICollider* const player) {
}

void BBulletToPlayerCallBacks::CollisionExit([[maybe_unused]] ICollider* const bullet, [[maybe_unused]] ICollider* const player) {
}

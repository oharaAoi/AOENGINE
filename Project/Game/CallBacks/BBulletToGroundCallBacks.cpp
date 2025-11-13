#include "BBulletToGroundCallBacks.h"
#include "Game/Information/ColliderCategory.h"

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BBulletToGroundCallBacks::Init() {
	SetCallBacks();
	SetPair(pCollisionManager_, ColliderTags::Boss::missile, ColliderTags::Field::ground);

	ParticleManager* manager = ParticleManager::GetInstance();
	hitEffect_ = manager->CrateParticle("MissileHit");
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BBulletToGroundCallBacks::Update() {
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ コリジョン処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BBulletToGroundCallBacks::CollisionEnter([[maybe_unused]] ICollider* const bullet, [[maybe_unused]] ICollider* const ground) {
	BaseBullet* hitBullet = pBossBulletManager_->SearchCollider(bullet);
	if (hitBullet != nullptr) {
		hitBullet->SetIsAlive(false);
		hitEffect_->SetPos(hitBullet->GetPosition());
		hitEffect_->Reset();
	}
}

void BBulletToGroundCallBacks::CollisionStay([[maybe_unused]] ICollider* const bullet, [[maybe_unused]] ICollider* const ground) {
}

void BBulletToGroundCallBacks::CollisionExit([[maybe_unused]] ICollider* const bullet, [[maybe_unused]] ICollider* const ground) {
}

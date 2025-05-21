#include "BBulletToGroundCallBacks.h"
#include "Game/Information/ColliderCategory.h"

void BBulletToGroundCallBacks::Init() {
	SetCallBacks();
	SetPair(pCollisionManager_, ColliderTags::Boss::missile, ColliderTags::Field::ground);
}

void BBulletToGroundCallBacks::Update() {
}

void BBulletToGroundCallBacks::CollisionEnter([[maybe_unused]] ICollider* const bullet, [[maybe_unused]] ICollider* const ground) {
	BaseBullet* hitBullet = pBossBulletManager_->SearchCollider(bullet);
	if (hitBullet != nullptr) {
		hitBullet->SetIsAlive(false);
	}
}

void BBulletToGroundCallBacks::CollisionStay([[maybe_unused]] ICollider* const bullet, [[maybe_unused]] ICollider* const ground) {
}

void BBulletToGroundCallBacks::CollisionExit([[maybe_unused]] ICollider* const bullet, [[maybe_unused]] ICollider* const ground) {
}

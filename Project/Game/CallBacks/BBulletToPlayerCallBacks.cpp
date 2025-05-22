#include "BBulletToPlayerCallBacks.h"
#include "Game/Information/ColliderCategory.h"

void BBulletToPlayerCallBacks::Init() {
	SetCallBacks();
	SetPair(pCollisionManager_, ColliderTags::Boss::missile, ColliderTags::Player::own);
}

void BBulletToPlayerCallBacks::Update() {
}

void BBulletToPlayerCallBacks::CollisionEnter([[maybe_unused]] ICollider* const bullet, [[maybe_unused]] ICollider* const player) {
	BaseBullet* hitBullet = pBossBulletManager_->SearchCollider(bullet);
	if (hitBullet != nullptr) {
		hitBullet->SetIsAlive(false);
	}

	//pPlayer_->Knockback((player->GetCenterPos() - bullet->GetCenterPos()).Normalize());
}

void BBulletToPlayerCallBacks::CollisionStay([[maybe_unused]] ICollider* const bullet, [[maybe_unused]] ICollider* const player) {
}

void BBulletToPlayerCallBacks::CollisionExit([[maybe_unused]] ICollider* const bullet, [[maybe_unused]] ICollider* const player) {
}

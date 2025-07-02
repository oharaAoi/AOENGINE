#include "PlayerToGroundCallBacks.h"
#include "Game/Information/ColliderCategory.h"

void PlayerToGroundCallBacks::Init() {
	SetCallBacks();
	SetPair(pCollisionManager_, ColliderTags::Player::leg, ColliderTags::None::own);
}

void PlayerToGroundCallBacks::Update() {
}

void PlayerToGroundCallBacks::CollisionEnter([[maybe_unused]] ICollider* const player, [[maybe_unused]] ICollider* const ground) {
	//pPlayer_->Landing();
}

void PlayerToGroundCallBacks::CollisionStay([[maybe_unused]] ICollider* const player, [[maybe_unused]] ICollider* const ground) {
}

void PlayerToGroundCallBacks::CollisionExit([[maybe_unused]] ICollider* const player, [[maybe_unused]] ICollider* const ground) {
}

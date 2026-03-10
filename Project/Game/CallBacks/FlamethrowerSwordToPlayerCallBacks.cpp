#include "FlamethrowerSwordToPlayerCallBacks.h"
#include "Game/Information/ColliderCategory.h"

void FlamethrowerSwordToPlayerCallBacks::Init() {
	SetCallBacks();
	SetPair(pCollisionManager_, ColliderTags::Bullet::flamethrowerSword, ColliderTags::Player::own);
}

void FlamethrowerSwordToPlayerCallBacks::Update() {
}

void FlamethrowerSwordToPlayerCallBacks::CollisionEnter([[maybe_unused]] AOENGINE::BaseCollider* const sword, [[maybe_unused]] AOENGINE::BaseCollider* const player) {
	const float damageValue = 30.0f;
	pPlayer_->Damage(damageValue);
}

void FlamethrowerSwordToPlayerCallBacks::CollisionStay([[maybe_unused]] AOENGINE::BaseCollider* const sword, [[maybe_unused]] AOENGINE::BaseCollider* const player) {
}

void FlamethrowerSwordToPlayerCallBacks::CollisionExit([[maybe_unused]] AOENGINE::BaseCollider* const sword, [[maybe_unused]] AOENGINE::BaseCollider* const player) {
}

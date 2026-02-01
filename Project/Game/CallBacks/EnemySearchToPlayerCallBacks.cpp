#include "EnemySearchToPlayerCallBacks.h"
#include "Game/Information/ColliderCategory.h"

void EnemySearchToPlayerCallBacks::Init() {
	SetCallBacks();
	SetPair(pCollisionManager_, ColliderTags::Player::own, ColliderTags::Enemy::searchCollider);
}

void EnemySearchToPlayerCallBacks::Update() {
}

void EnemySearchToPlayerCallBacks::CollisionEnter([[maybe_unused]] AOENGINE::BaseCollider* const player, [[maybe_unused]] AOENGINE::BaseCollider* const enemy) {
	BaseEnemy* pEnemy = pEnemyManager_->SearchCollider(enemy);
	if (pEnemy) {
		pEnemy->SetIsTargetDiscovery(true);
		pEnemy->SetTargetTransform(pPlayer_->GetTransform());
	}
}

void EnemySearchToPlayerCallBacks::CollisionStay([[maybe_unused]] AOENGINE::BaseCollider* const player, [[maybe_unused]] AOENGINE::BaseCollider* const enemy) {
}

void EnemySearchToPlayerCallBacks::CollisionExit([[maybe_unused]] AOENGINE::BaseCollider* const player, [[maybe_unused]] AOENGINE::BaseCollider* const enemy) {
	
}

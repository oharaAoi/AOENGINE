#include "CollisionCallBacks.h"

#include "Game/Actor/Floor/DamageFloor.h"

void CollisionCallBacks::Init(AOENGINE::CollisionManager* collisionManager, Player* player, DamageFloor* damageFloor, Boss* boss) {
	playerToDamageFloor_ = std::make_unique<PlayerToDamageFloorCallBacks>(collisionManager);
	playerToDamageFloor_->Init();
	playerToDamageFloor_->SetPlayer(player);
	playerToDamageFloor_->SetDamageFloor(damageFloor);

	// ボスの攻撃(火球・ビーム)がプレイヤーに当たった時のダメージ
	playerToBossAttack_ = std::make_unique<PlayerToBossAttackCallBacks>(collisionManager);
	playerToBossAttack_->SetPlayer(player);
	playerToBossAttack_->SetBoss(boss);
	playerToBossAttack_->Init();
}

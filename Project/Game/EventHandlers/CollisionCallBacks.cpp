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

	// 足止めにブロックの塊が当たった時のSE
	stopperToBlockGroup_ = std::make_unique<StopperToBlockGroupCallBacks>(collisionManager);
	stopperToBlockGroup_->SetLauncherManager(player->GetBlockGroupLauncherManagerRef());
	stopperToBlockGroup_->Init();
}

#include "CollisionCallBacks.h"

#include "Game/Actor/Floor/DamageFloor.h"

void CollisionCallBacks::Init(AOENGINE::CollisionManager* collisionManager, Player* player, DamageFloor* damageFloor) {
	playerToDamageFloor_ = std::make_unique<PlayerToDamageFloorCallBacks>(collisionManager);
	playerToDamageFloor_->Init();
	playerToDamageFloor_->SetPlayer(player);
	playerToDamageFloor_->SetDamageFloor(damageFloor);
}

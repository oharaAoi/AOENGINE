#include "CollisionCallBacks.h"

void CollisionCallBacks::Init(AOENGINE::CollisionManager* collisionManager, Player* player) {
	playerToDamageFloor_ = std::make_unique<PlayerToDamageFloorCallBacks>(collisionManager);
	playerToDamageFloor_->Init();
	playerToDamageFloor_->SetPlayer(player);
}

#include "BaseCollisionCallBacks.h"
#include "Engine/System/Manager/CollisionLayerManager.h"

void BaseCollisionCallBacks::SetCallBacks() {
	callBacks_.enter = std::bind(&BaseCollisionCallBacks::CollisionEnter, this, __CALLBACK_PLACEHOLDERS_12);
	callBacks_.stay = std::bind(&BaseCollisionCallBacks::CollisionStay, this, __CALLBACK_PLACEHOLDERS_12);
	callBacks_.exit = std::bind(&BaseCollisionCallBacks::CollisionExit, this, __CALLBACK_PLACEHOLDERS_12);
}

void BaseCollisionCallBacks::SetPair(CollisionManager* collisionManager, const std::string& attacker, const std::string& defense) {
	CollisionLayerManager& layerManager = CollisionLayerManager::GetInstance();
	collisionManager->MakeCollisionPair(layerManager.GetCategoryBit(attacker), layerManager.GetCategoryBit(defense), callBacks_);
}

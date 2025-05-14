#include "GameCallBacksManager.h"

void GameCallBacksManager::Init(CollisionManager* _manager) {
	pCollisionManager_ = _manager;

	// -------------------------------------------------
	// ↓ PlayerBullet To
	// -------------------------------------------------
	pBulletToBoss_ = std::make_unique<PBulletToBossCallBacks>(pCollisionManager_);
	pBulletToBoss_->Init();
	pBulletToBoss_->SetBoss(pBoss_);
	pBulletToBoss_->SetBulletManager(pPlayerManager_->GetBulletManager());

	// -------------------------------------------------
	// ↓ Player To
	// -------------------------------------------------

	pPlayerToGround_ = std::make_unique<PlayerToGroundCallBacks>(pCollisionManager_);
	pPlayerToGround_->Init();
	pPlayerToGround_->SetPlayer(pPlayerManager_->GetPlayer());
	pPlayerToGround_->SetGround(pFloor_);

}

void GameCallBacksManager::Update() {
	pBulletToBoss_->Update();
}

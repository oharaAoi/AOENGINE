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
}

void GameCallBacksManager::Update() {
	pBulletToBoss_->Update();
}

#include "GameCallBacksManager.h"

void GameCallBacksManager::Init(CollisionManager* _manager) {
	pCollisionManager_ = _manager;

	// -------------------------------------------------
	// ↓ PlayerBullet To
	// -------------------------------------------------
	pBulletToBoss_ = std::make_unique<PBulletToBossCallBacks>(pCollisionManager_);
	pBulletToBoss_->Init();
	pBulletToBoss_->SetBoss(pBossRoot_->GetBoss());
	pBulletToBoss_->SetBulletManager(pPlayerManager_->GetBulletManager());

	// -------------------------------------------------
	// ↓ Player To
	// -------------------------------------------------

	pPlayerToGround_ = std::make_unique<PlayerToGroundCallBacks>(pCollisionManager_);
	pPlayerToGround_->Init();
	pPlayerToGround_->SetPlayer(pPlayerManager_->GetPlayer());
	pPlayerToGround_->SetGround(pFloor_);

	// -------------------------------------------------
	// ↓ BossBullet To
	// -------------------------------------------------

	pBBulletToPlayer_ = std::make_unique<BBulletToPlayerCallBacks>(pCollisionManager_);
	pBBulletToPlayer_->Init();
	pBBulletToPlayer_->SetBossBulletManager(pBossRoot_->GetBulletManager());
	pBBulletToPlayer_->SetPlayer(pPlayerManager_->GetPlayer());

}

void GameCallBacksManager::Update() {
	pBulletToBoss_->Update();
}

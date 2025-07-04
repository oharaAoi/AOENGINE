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

	// toGround
	pBBulletToGround_ = std::make_unique<BBulletToGroundCallBacks>(pCollisionManager_);
	pBBulletToGround_->Init();
	pBBulletToGround_->SetBossBulletManager(pBossRoot_->GetBulletManager());
	pBBulletToGround_->SetGround(pFloor_);

}

void GameCallBacksManager::Update() {
	pBBulletToPlayer_->Update();
	pBBulletToGround_->Update();
	pBulletToBoss_->Update();
}

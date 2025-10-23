#include "BossRoot.h"

void BossRoot::Init() {
	boss_ = std::make_unique<Boss>();
	boss_->Init();

	bulletManager_ = std::make_unique<BossBulletManager>();
	bulletManager_->Init();

	// 初期設定
	boss_->SetBulletManager(bulletManager_.get());
}

void BossRoot::Update() {
	boss_->SetIsTargetDead(pPlayer_->GetIsDead());
	boss_->SetPlayerPosition(pPlayer_->GetTransform()->GetPos());
	boss_->Update();

	bulletManager_->SetPlayerPosition(pPlayer_->GetTransform()->GetPos());
	bulletManager_->Update();
}
#include "BossRoot.h"
#include "BossRoot.h"
#include "BossRoot.h"

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossRoot::Init() {
	boss_ = std::make_unique<Boss>();
	boss_->Init();

	bulletManager_ = std::make_unique<BossBulletManager>();
	bulletManager_->Init();

	// 初期設定
	boss_->SetBulletManager(bulletManager_.get());
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossRoot::Update() {
	if (pPlayer_->GetIsDead()) {
		boss_->GetBehaviorTree()->SetExecute(false);
	}

	boss_->SetIsTargetDead(pPlayer_->GetIsDead());
	boss_->Update();

	bulletManager_->SetPlayerPosition(pPlayer_->GetTransform()->GetPos());
	bulletManager_->Update();
}
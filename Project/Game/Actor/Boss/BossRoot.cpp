#include "BossRoot.h"
#include "Engine/System/Editor/Window/EditorWindows.h"

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossRoot::Init() {
	bulletManager_ = std::make_unique<BossBulletManager>();
	bulletManager_->Init();

	AOENGINE::EditorWindows::AddObjectWindow(this, "BossRoot");
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossRoot::Update() {
	if (boss_) {
		if (pPlayer_->GetIsDead()) {
			boss_->GetBehaviorTree()->SetExecute(false);
		}

		boss_->SetIsTargetDead(pPlayer_->GetIsDead());
	}

	bulletManager_->SetPlayerPosition(pPlayer_->GetTransform()->GetWorldPos());
	bulletManager_->Update();
}

void BossRoot::Debug_Gui() {
	
}

void BossRoot::SetBoss(Boss* boss) {
	boss_ = boss;
	boss_->SetBulletManager(bulletManager_.get());
}
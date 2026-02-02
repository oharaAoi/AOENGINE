#include "CanvasUI.h"
#include "Engine/System/Editor/Window/EditorWindows.h"
#include "Engine/System/Input/Input.h"
#include <Game/Commands/LockOn/LockOnCommands.h>
#include "Game/Actor/Enemy/BaseEnemy.h"

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void CanvasUI::Init(bool _isTutorial) {
	isTutorial_ = _isTutorial;

	reticle_ = std::make_unique<Reticle>();
	reticle_->Init();

	// player
	playerUIs_ = std::make_unique<PlayerUIs>();
	playerUIs_->Init(pPlayer_);

	// boss
	bossUIs_ = std::make_unique<BossUIs>();
	bossUIs_->Init(pBoss_, pPlayer_);

	targetUI_ = std::make_unique<TargetUI>();
	targetUI_->Init();

	// out game
	clearNotificationUI_ = std::make_unique<ClearNotificationUI>();
	clearNotificationUI_->Init();
	if (isTutorial_) {
		clearNotificationUI_->GetSprite()->SetEnable(false);
	}

	lockOnInvoker_.Register("lockOn", lockOnInvoker_.MakeFactory<LockOnCommand>(pEnemyManager_, reticle_.get()));

	AddChild(bossUIs_.get());
	AddChild(playerUIs_.get());
	AddChild(clearNotificationUI_.get());
	
	AOENGINE::EditorWindows::AddObjectWindow(this, "Canvas");
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void CanvasUI::Update() {
	if (AOENGINE::Input::GetInstance()->IsTriggerButton(XInputButtons::RStickThumb)) {
		if (!reticle_->GetLockOn()) {
			lockOnInvoker_.Invoke("lockOn");
		} else {
			reticle_->ReleaseLockOn();
			pEnemyManager_->SetNearReticleEnemy(nullptr);

			bossUIs_->SetIsEnable(false);
			targetUI_->SetIsEnable(false);
		}
	}

	if (pBoss_->GetIsBreak()) {
		reticle_->ReleaseLockOn();
	}

	BaseEnemy* targetEnemy = pEnemyManager_->GetNearReticleEnemy();
	if (targetEnemy) {
		if (targetEnemy->GetEnemyType() == EnemyType::Boss) {
			reticle_->SetReticlePos(pBoss_->GetTransform(), pFollowCamera_->GetVpvpMatrix());

			bossUIs_->SetIsEnable(true);
			targetUI_->SetIsEnable(false);
			bossUIs_->Update(reticle_->GetPos());
			targetUI_->Update(reticle_->GetPos(), targetEnemy);
		} else {
			AOENGINE::WorldTransform* transform = pEnemyManager_->GetNearReticleEnemy()->GetTransform();
			reticle_->SetReticlePos(transform, pFollowCamera_->GetVpvpMatrix());

			bossUIs_->SetIsEnable(false);
			targetUI_->SetIsEnable(true);
			targetUI_->Update(reticle_->GetPos(), targetEnemy);
			bossUIs_->Update(reticle_->GetPos());
		}
	}

	reticle_->Update();
	
	// -------------------------------------------------
	// ↓ 各更新処理
	// -------------------------------------------------

	// player
	playerUIs_->Update(reticle_->GetPos());

	// out game
	if (!isTutorial_) {
		clearNotificationUI_->Update(pBoss_->GetIsBreak());
	}

	bossUIs_->AlertUpdate();
}

void CanvasUI::PostUpdate() {
	BaseEnemy* targetEnemy = pEnemyManager_->GetNearReticleEnemy();
	if (!targetEnemy) { return; }
	if (targetEnemy->GetIsDead()) {
		pEnemyManager_->SetNearReticleEnemy(nullptr);
		if (targetEnemy->GetEnemyType() == EnemyType::Boss) {
			bossUIs_->SetIsEnable(false);
		} else {
			targetUI_->SetIsEnable(false);
		}
		reticle_->ReleaseLockOn();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 描画処理
///////////////////////////////////////////////////////////////////////////////////////////////

void CanvasUI::Draw() const {
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集処理
///////////////////////////////////////////////////////////////////////////////////////////////

void CanvasUI::Debug_Gui() {
	ImGui::DragFloat2("boostOnPos_", &boostOnPos_.x);
	ImGui::DragFloat2("boostOnScale_", &boostOnScale_.x);
}
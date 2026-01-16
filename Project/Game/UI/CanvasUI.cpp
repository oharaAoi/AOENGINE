#include "CanvasUI.h"
#include "Engine/System/Editor/Window/EditorWindows.h"
#include "Engine/Module/Components/2d/Canvas2d.h"

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

	// out game
	clearNotificationUI_ = std::make_unique<ClearNotificationUI>();
	clearNotificationUI_->Init();
	if (isTutorial_) {
		clearNotificationUI_->GetSprite()->SetEnable(false);
	}

	AddChild(bossUIs_.get());
	AddChild(playerUIs_.get());
	AddChild(clearNotificationUI_.get());
	
	AOENGINE::EditorWindows::AddObjectWindow(this, "Canvas");
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void CanvasUI::Update() {
	if (pBoss_->GetIsBreak()) {
		reticle_->ReleaseLockOn();
	}
	reticle_->Update(pBoss_->GetTransform()->GetWorldMatrix(), pFollowCamera_->GetVpvpMatrix());
	
	// -------------------------------------------------
	// ↓ 各更新処理
	// -------------------------------------------------
	
	// boss
	bossUIs_->Update(reticle_->GetPos(), reticle_->GetLockOn());

	// player
	playerUIs_->Update(reticle_->GetPos());

	// out game
	if (!isTutorial_) {
		clearNotificationUI_->Update(pBoss_->GetIsBreak());
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
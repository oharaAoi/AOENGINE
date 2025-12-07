#include "TutorialCanvas.h"
#include "Engine/System/Editer/Window/EditorWindows.h"
#include "Engine/Module/Components/2d/Canvas2d.h"

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void TutorialCanvas::Init() {
	AOENGINE::Canvas2d* canvas = Engine::GetCanvas2d();

	reticle_ = std::make_unique<Reticle>();
	reticle_->Init();

	boostOn_ = canvas->AddSprite("boostOn.png", "boostOn");

	// player
	playerUIs_ = std::make_unique<PlayerUIs>();
	playerUIs_->Init(pPlayer_);

	AddChild(playerUIs_.get());

	AOENGINE::EditorWindows::AddObjectWindow(this, "Canvas");
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void TutorialCanvas::Update() {
	if (pBoss_->GetIsBreak()) {
		reticle_->ReleaseLockOn();
	}
	reticle_->Update(pBoss_->GetTransform()->GetWorldMatrix(), pFollowCamera_->GetVpvpMatrix());

	// -------------------------------------------------
	// ↓ 各更新処理
	// -------------------------------------------------

	boostOn_->SetTranslate(boostOnPos_);
	boostOn_->SetScale(boostOnScale_);
	if (pPlayer_->GetIsBoostMode()) {
		boostOn_->SetEnable(true);
	} else {
		boostOn_->SetEnable(false);
	}

	// boss
	playerUIs_->Update(reticle_->GetPos());
}

void TutorialCanvas::Debug_Gui() {
}

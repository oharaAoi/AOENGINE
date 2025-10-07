#include "TutorialCanvas.h"
#include "Engine/System/Editer/Window/EditorWindows.h"
#include "Engine/Module/Components/2d/Canvas2d.h"

void TutorialCanvas::Init() {
	Canvas2d* canvas = Engine::GetCanvas2d();

	reticle_ = std::make_unique<Reticle>();
	reticle_->Init();

	boostOn_ = Engine::CreateSprite("boostOn.png");

	// player
	playerUIs_ = std::make_unique<PlayerUIs>();
	playerUIs_->Init(pPlayer_);

	canvas->AddSprite(boostOn_.get());

	AddChild(playerUIs_.get());

	EditorWindows::AddObjectWindow(this, "Canvas");
}

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

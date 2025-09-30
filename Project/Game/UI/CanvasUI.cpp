#include "CanvasUI.h"
#include "Engine/System/Editer/Window/EditorWindows.h"
#include "Engine/Module/Components/2d/Canvas2d.h"

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void CanvasUI::Init() {
	Canvas2d* canvas = Engine::GetCanvas2d();

	reticle_ = std::make_unique<Reticle>();
	reticle_->Init();

	energyOutput_ = std::make_unique<EnergyOutput>();
	energyOutput_->Init();

	boostOn_ = Engine::CreateSprite("boostOn.png");

	// player
	playerUIs_ = std::make_unique<PlayerUIs>();
	playerUIs_->Init(pPlayer_);

	// boss
	bossUIs_ = std::make_unique<BossUIs>();
	bossUIs_->Init(pBoss_);

	// out game
	clearNotificationUI_ = std::make_unique<ClearNotificationUI>();
	clearNotificationUI_->Init();

	control_ = Engine::CreateSprite("control.png");
	control_->SetTranslate(Vector2(640.0f, 360.0f));

	canvas->AddSprite(boostOn_.get());
	canvas->AddSprite(control_.get());

	AddChild(energyOutput_.get());
	AddChild(bossUIs_.get());
	AddChild(playerUIs_.get());
	AddChild(clearNotificationUI_.get());
	
	EditorWindows::AddObjectWindow(this, "Canvas");
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void CanvasUI::Update() {
	if (pBoss_->GetIsBreak()) {
		reticle_->ReleaseLockOn();
	}
	reticle_->Update(pBoss_->GetTransform()->GetWorldMatrix(), pFollowCamera_->GetVpvpMatrix());
	
	const Player::Parameter& playerParam = pPlayer_->GetParam();
	const Player::Parameter& playerInitParam = pPlayer_->GetInitParam();

	// -------------------------------------------------
	// ↓ 各更新処理
	// -------------------------------------------------

	energyOutput_->Update(playerParam.energy / playerInitParam.energy);
	//postureStability_->Update();

	boostOn_->SetTranslate(boostOnPos_);
	boostOn_->SetScale(boostOnScale_);
	if (pPlayer_->GetIsBoostMode()) {
		boostOn_->SetEnable(true);
	} else {
		boostOn_->SetEnable(false);
	}
	
	// boss
	bossUIs_->Update();

	playerUIs_->Update(reticle_->GetPos());

	// out game
	clearNotificationUI_->Update(pBoss_->GetIsBreak());
	control_->Update();
	
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
	boostOn_->Debug_Gui();
	ImGui::DragFloat2("boostOnPos_", &boostOnPos_.x);
	ImGui::DragFloat2("boostOnScale_", &boostOnScale_.x);
}
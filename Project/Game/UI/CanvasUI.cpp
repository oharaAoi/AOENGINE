#include "CanvasUI.h"
#include "Engine/System/Editer/Window/EditerWindows.h"

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void CanvasUI::Init() {
	reticle_ = std::make_unique<Reticle>();
	reticle_->Init();

	energyOutput_ = std::make_unique<EnergyOutput>();
	energyOutput_->Init();

	postureStability_ = std::make_unique<PostureStability>();
	postureStability_->Init();

	boostOn_ = Engine::CreateSprite("boostOn.png");

	AddChild(energyOutput_.get());

#ifdef _DEBUG
	EditerWindows::AddObjectWindow(this, "Canvas");
#endif // _DEBUG
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void CanvasUI::Update() {
	reticle_->Update(pBoss_->GetTransform()->GetWorldMatrix(), pFollowCamera_->GetVpvpMatrix());
	
	const Player::Parameter& playerParam = pPlayer_->GetParam();
	const Player::Parameter& playerInitParam = pPlayer_->GetInitParam();
	energyOutput_->Update(playerParam.energy / playerInitParam.energy);
	//postureStability_->Update();

	boostOn_->SetTranslate(boostOnPos_);
	boostOn_->SetScale(boostOnScale_);
	boostOn_->Update();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 描画処理
///////////////////////////////////////////////////////////////////////////////////////////////

void CanvasUI::Draw() const {
	reticle_->Draw();

	energyOutput_->Draw();

	if (pPlayer_->GetIsBoostMode()) {
		boostOn_->Draw();
	} 
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集処理
///////////////////////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
void CanvasUI::Debug_Gui() {
	boostOn_->Debug_Gui();
	ImGui::DragFloat2("boostOnPos_", &boostOnPos_.x);
	ImGui::DragFloat2("boostOnScale_", &boostOnScale_.x);
}
#endif
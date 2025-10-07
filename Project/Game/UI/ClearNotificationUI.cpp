#include "ClearNotificationUI.h"
#include "Engine.h"
#include "Engine/Lib/Json/JsonItems.h"

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void ClearNotificationUI::Init() {
	SetName("ClearNotificationUI");
	Canvas2d* canvas = Engine::GetCanvas2d();
	sprite_ = canvas->AddSprite("missionComplete.png", "missionComplete");
	param_.Load();

	posTween_.Init(param_.startPos, param_.endPos, param_.duration, (int)EasingType::In::Expo, LoopType::STOP);
	alphaTween_.Init(0.0f, 1.0f, param_.duration, (int)EasingType::In::Expo, LoopType::STOP);

	isBreak_ = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void ClearNotificationUI::Update(bool isBossBreak) {
	isBreak_ = isBossBreak;
	if (!isBossBreak) { 
		sprite_->SetEnable(false);
		return;
	}
	sprite_->SetEnable(true);

	posTween_.Update(GameTimer::DeltaTime());
	alphaTween_.Update(GameTimer::DeltaTime());

	sprite_->SetTranslate(posTween_.GetValue());
	sprite_->SetColor(Color(1.0f, 1.0f, 1.0f, alphaTween_.GetValue()));
	sprite_->Update();

	if (posTween_.GetIsFinish()) {
		
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 描画処理
///////////////////////////////////////////////////////////////////////////////////////////////

void ClearNotificationUI::Draw() const {
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集処理
///////////////////////////////////////////////////////////////////////////////////////////////

void ClearNotificationUI::Debug_Gui() {
	ImGui::DragFloat2("pos_", &pos_.x);
	if (ImGui::Button("Reset")) {
		Reset();
	}

	param_.Debug_Gui();
}

void ClearNotificationUI::Parameter::Debug_Gui() {
	ImGui::DragFloat2("startPos", &startPos.x);
	ImGui::DragFloat2("endPos", &endPos.x);
	ImGui::DragFloat("duration", &duration);
	SaveAndLoad();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 再生処理
///////////////////////////////////////////////////////////////////////////////////////////////

void ClearNotificationUI::Reset() {
	posTween_.Reset();
	alphaTween_.Reset();
}
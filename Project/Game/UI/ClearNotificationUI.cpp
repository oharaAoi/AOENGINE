#include "ClearNotificationUI.h"
#include "Engine.h"
#include "Engine/Lib/Json/JsonItems.h"

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void ClearNotificationUI::Init() {
	SetName("ClearNotificationUI");
	sprite_ = Engine::CreateSprite("missionComplete.png");
	param_.FromJson(JsonItems::GetData(GetName(), param_.GetName()));

	posTween_.Init(&pos_, param_.startPos, param_.endPos, param_.duration, (int)EasingType::In::Expo, LoopType::STOP);
	alphaTween_.Init(&alpha_, 0.0f, 1.0f, param_.duration, (int)EasingType::In::Expo, LoopType::STOP);

	isBreak_ = false;

	Engine::GetCanvas2d()->AddSprite(sprite_.get());
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

	sprite_->SetTranslate(pos_);
	sprite_->SetColor(Vector4(1.0f, 1.0f, 1.0f, alpha_));
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

	if (ImGui::CollapsingHeader("Parameter")) {
		ImGui::DragFloat2("startPos", &param_.startPos.x);
		ImGui::DragFloat2("endPos", &param_.endPos.x);
		ImGui::DragFloat("duration", &param_.duration);

		if (ImGui::Button("Save")) {
			JsonItems::Save(GetName(), param_.ToJson(param_.GetName()));
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 再生処理
///////////////////////////////////////////////////////////////////////////////////////////////

void ClearNotificationUI::Reset() {
	posTween_.Reset();
	alphaTween_.Reset();
}
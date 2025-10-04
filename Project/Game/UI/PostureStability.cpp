#include "PostureStability.h"
#include "Engine.h"
#include "Engine/Lib/Json/JsonItems.h"

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void PostureStability::Init(const std::string& _groupName, const std::string& _itemName) {
	SetName("PostureStability");
	groupName_ = _groupName;
	BaseGaugeUI::Init("postureStability_bg.png", "postureStability_front.png");
	front_->Load(_groupName, _itemName);

	// 柵の初期化
	fence_ = Engine::CreateSprite("postureStability_fence.png");
	fence_->SetTranslate(front_->GetTranslate());
	fence_->SetScale(front_->GetScale());

	param_.FromJson(JsonItems::GetData("PostureStability", param_.GetName()));
	Engine::GetCanvas2d()->AddSprite(fence_.get());
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void PostureStability::Update(float _fillAmount) {
	fillAmount_ = _fillAmount;
	Color color = Color::Lerp(param_.normalColor, param_.pinchColor, fillAmount_);
	front_->SetColor(color);

	BaseGaugeUI::Update();

	fence_->SetTranslate(front_->GetTranslate());
	fence_->SetScale(front_->GetScale());

	fence_->Update();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集処理
///////////////////////////////////////////////////////////////////////////////////////////////

void PostureStability::Debug_Gui() {
	BaseGaugeUI::Debug_Gui();

	if (ImGui::CollapsingHeader("Parameter")) {
		ImGui::ColorEdit4("normalColor", &param_.normalColor.r);
		ImGui::ColorEdit4("pinchColor", &param_.pinchColor.r);

		if (ImGui::Button("Save")) {
			JsonItems::Save(GetName(), param_.ToJson(param_.GetName()));
		}
	}
}
#include "BossUIs.h"
#include "Engine/Lib/Json/JsonItems.h"

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossUIs::Init(Boss* _boss) {
	SetName("BossUIs");
	health_ = std::make_unique<BossHealthUI>();
	health_->Init();

	postureStability_ = std::make_unique<PostureStability>();
	postureStability_->Init();

	pBoss_ = _boss;

	uiItems_.FromJson(JsonItems::GetData(GetName(), uiItems_.GetName()));

	health_->SetScale(uiItems_.healthScale);
	health_->SetCenterPos(uiItems_.healthPos);

	postureStability_->SetScale(uiItems_.postureScale);
	postureStability_->SetCenterPos(uiItems_.posturePos);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossUIs::Update() {
	const Boss::Parameter& bossParam = pBoss_->GetParameter();
	const Boss::Parameter& bossInitParam = pBoss_->GetInitParameter();

	health_->Update(bossParam.health/ bossInitParam.health);
	postureStability_->Update(bossParam.postureStability / bossInitParam.postureStability);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 描画処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossUIs::Draw() const {
	health_->Draw();

	postureStability_->Draw();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集
///////////////////////////////////////////////////////////////////////////////////////////////

void BossUIs::Debug_Gui() {
	if (ImGui::CollapsingHeader("Health")) {
		ImGui::DragFloat2("HealthScale", &uiItems_.healthScale.x, 0.1f);
		ImGui::DragFloat2("HealthPos", &uiItems_.healthPos.x, 0.1f);

		health_->SetScale(uiItems_.healthScale);
		health_->SetCenterPos(uiItems_.healthPos);
	}

	if (ImGui::CollapsingHeader("PostureStability")) {
		ImGui::DragFloat2("postureScale", &uiItems_.postureScale.x, 0.1f);
		ImGui::DragFloat2("posturePos", &uiItems_.posturePos.x, 0.1f);

		postureStability_->SetScale(uiItems_.postureScale);
		postureStability_->SetCenterPos(uiItems_.posturePos);
	}


	if (ImGui::Button("Save")) {
		JsonItems::Save(GetName(), uiItems_.ToJson(uiItems_.GetName()));
	}
}
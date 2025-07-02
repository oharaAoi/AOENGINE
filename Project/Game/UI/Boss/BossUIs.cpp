#include "BossUIs.h"
#include "Engine/Lib/Json/JsonItems.h"

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossUIs::Init(Boss* _boss) {
	SetName("BossUIs");
	pBoss_ = _boss;
	uiItems_.FromJson(JsonItems::GetData(GetName(), uiItems_.GetName()));

	health_ = std::make_unique<BossHealthUI>();
	health_->Init();

	postureStability_ = std::make_unique<PostureStability>();
	postureStability_->Init();

	armorDurability_ = std::make_unique<ArmorDurabilityUI>();
	armorDurability_->Init();

	stanGaugeUI_ = std::make_unique<StanGaugeUI>();
	stanGaugeUI_->Init(uiItems_.postureScale, uiItems_.posturePos);

	health_->SetScale(uiItems_.healthScale);
	health_->SetCenterPos(uiItems_.healthPos);

	postureStability_->SetScale(uiItems_.postureScale);
	postureStability_->SetCenterPos(uiItems_.posturePos);

	armorDurability_->SetScale(uiItems_.postureScale);
	armorDurability_->SetCenterPos(uiItems_.posturePos);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossUIs::Update() {
	const Boss::Parameter& bossParam = pBoss_->GetParameter();
	const Boss::Parameter& bossInitParam = pBoss_->GetInitParameter();

	// hp
	health_->Update(bossParam.health/ bossInitParam.health);

	// 耐久度
	if (pBoss_->GetPulseArmor()->GetIsAlive()) {
		armorDurability_->Update(pBoss_->GetPulseArmor()->ArmorDurability());
	} else {
		postureStability_->Update(bossParam.postureStability / bossInitParam.postureStability);
	}

	// スタン
	if (pBoss_->GetIsStan()) {
		stanGaugeUI_->Update();
	}

	// 警告
	for (auto it = attackAlertList_.begin(); it != attackAlertList_.end();) {
		if ((*it)->IsDestroy()) {
			it = attackAlertList_.erase(it);
		} else {
			++it;
		}
	}

	for (auto& alert : attackAlertList_) {
		alert->Update();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 描画処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossUIs::Draw() const {
	health_->Draw();

	if (!pBoss_->GetPulseArmor()->GetIsAlive()) {
		postureStability_->Draw();
	} else {
		armorDurability_->Draw();
	}

	for (auto& alert : attackAlertList_) {
		alert->Draw();
	}

	// stanゲージ
	if (pBoss_->GetIsStan()) {
		stanGaugeUI_->Draw();
	}
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
		postureStability_->Debug_Gui();

		postureStability_->SetScale(uiItems_.postureScale);
		postureStability_->SetCenterPos(uiItems_.posturePos);
	}

	if (ImGui::CollapsingHeader("Stan")) {
		stanGaugeUI_->Debug_Gui();
	}


	if (ImGui::Button("Save")) {
		JsonItems::Save(GetName(), uiItems_.ToJson(uiItems_.GetName()));
	}
}

void BossUIs::PopAlert() {
	auto& alert = attackAlertList_.emplace_back(std::make_unique<EnemyAttackAlert>());
	alert->Init();
}

void BossUIs::PopStan() {
	stanGaugeUI_->Pop();
}

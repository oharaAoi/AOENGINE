#include "BossUIs.h"
#include "Engine/Lib/Json/JsonItems.h"

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossUIs::Init(Boss* _boss) {
	SetName("BossUIs");
	pBoss_ = _boss;

	health_ = std::make_unique<BossHealthUI>();
	health_->Init("BossUIs", "BossHealthUI");

	postureStability_ = std::make_unique<PostureStability>();
	postureStability_->Init("BossUIs", "PostureStability");

	armorDurability_ = std::make_unique<ArmorDurabilityUI>();
	armorDurability_->Init("BossUIs", "ArmorDurabilityUI");

	stanGaugeUI_ = std::make_unique<StanGaugeUI>();
	stanGaugeUI_->Init("BossUIs", "StanGaugeUI");

	armorDurability_->SetIsEnable(false);
	stanGaugeUI_->SetIsEnable(false);

	AddChild(health_.get());
	AddChild(postureStability_.get());
	AddChild(armorDurability_.get());
	AddChild(stanGaugeUI_.get());

	EditorWindows::AddObjectWindow(this, "BossUIs");
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
		stanGaugeUI_->Update(0);
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

	if (!pBoss_->GetPulseArmor()->GetIsAlive()) {
		armorDurability_->SetIsEnable(false);
		postureStability_->SetIsEnable(true);
	} else {
		postureStability_->SetIsEnable(false);
		armorDurability_->SetIsEnable(true);
	}

	if (pBoss_->GetIsStan()) {
		stanGaugeUI_->SetIsEnable(true);
	} else {
		stanGaugeUI_->SetIsEnable(false);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集
///////////////////////////////////////////////////////////////////////////////////////////////

void BossUIs::Debug_Gui() {
}

void BossUIs::PopAlert() {
	auto& alert = attackAlertList_.emplace_back(std::make_unique<EnemyAttackAlert>());
	alert->Init();
}

void BossUIs::PopStan() {
	stanGaugeUI_->Pop();
}

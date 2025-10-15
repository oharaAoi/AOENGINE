#include "BossUIs.h"
#include "Engine/Lib/Math/MyMath.h"
#include "Engine/Lib/Json/JsonItems.h"

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossUIs::Init(Boss* _boss, Player* _player) {
	SetName("BossUIs");
	pBoss_ = _boss;
	pPlayer_ = _player;

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

void BossUIs::PopAlert(const Vector3& _targetPos, const Vector3& _attackerPos) {
	// 前方方向ベクトル
	Vector3 forward = pPlayer_->GetTransform()->GetRotate().MakeForward();
	forward.y = 0;
	forward = forward.Normalize();

	// 右方向ベクトル
	Vector3 right = pPlayer_->GetTransform()->GetRotate().MakeRight();
	right.y = 0;
	right = right.Normalize();

	// Targetへのベクトル
	Vector3 toEnemy = (_attackerPos - _targetPos).Normalize();

	// 4方向を用意
	std::array<std::pair<AttackAlertDirection, Vector3>, 4> dirs = { {
		{AttackAlertDirection::Front, forward},
		{AttackAlertDirection::Back,  forward * -1.0f},
		{AttackAlertDirection::Right, right},
		{AttackAlertDirection::Left,  right * -1.0f}
	} };

	// Dot値が最大の方向を選ぶ
	float bestDot = -9999.0f;
	AttackAlertDirection bestDir = AttackAlertDirection::Front;

	for (auto& [dir, vec] : dirs) {
		float dot = Vector3::Dot(toEnemy, vec);
		if (dot > bestDot) {
			bestDot = dot;
			bestDir = dir;
		}
	}

	auto& alert = attackAlertList_.emplace_back(std::make_unique<EnemyAttackAlert>());
	alert->Init(bestDir);
}

void BossUIs::PopStan() {
	stanGaugeUI_->Pop();
}

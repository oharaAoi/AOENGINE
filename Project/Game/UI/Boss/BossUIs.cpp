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
	// targetと攻撃者の方向を求める
	Vector3 toEnemy = (_attackerPos, _targetPos).Normalize();
	Vector3 forward = pPlayer_->GetTransform()->GetRotate().MakeForward();
	Vector3 right = Normalize(Vector3::Cross({0,1,0}, forward));
	
	// y軸は考慮しない
	toEnemy.y = 0;
	// 内積を割合化する
	float dotF = std::clamp(Dot(forward, toEnemy), -1.0f, 1.0f);
	float dotR = std::clamp(Dot(right, toEnemy), -1.0f, 1.0f);

	// clampをする
	float forwardRatio = std::max(0.0f, dotF);  // 前方向
	float backwardRatio = std::max(0.0f, -dotF);  // 後方向
	float rightRatio = std::max(0.0f, dotR);  // 右方向
	float leftRatio = std::max(0.0f, -dotR);  // 左方向

	// 正規化をする
	float sum = forwardRatio + backwardRatio + rightRatio + leftRatio;
	if (sum > 0.0f) {
		forwardRatio /= sum;
		backwardRatio /= sum;
		rightRatio /= sum;
		leftRatio /= sum;
	}

	// 最大方向を決定する
	float ratios[4] = { forwardRatio, backwardRatio, rightRatio, leftRatio };
	int maxIndex = 0;
	for (int i = 0; i < 4; ++i) {
		if (ratios[i] > ratios[maxIndex]) {
			maxIndex = i;
		}
	}
	// directionに代入する
	AttackAlertDirection alertDirection = AttackAlertDirection::Front;
	if (maxIndex == (int)AttackAlertDirection::Front) {
		alertDirection = AttackAlertDirection::Front;

	} else if (maxIndex == (int)AttackAlertDirection::Back) {
		alertDirection = AttackAlertDirection::Back;

	} else if (maxIndex == (int)AttackAlertDirection::Right) {
		alertDirection = AttackAlertDirection::Right;

	} else if (maxIndex == (int)AttackAlertDirection::Left) {
		alertDirection = AttackAlertDirection::Left;
	}

	// alertを出す
	auto& alert = attackAlertList_.emplace_back(std::make_unique<EnemyAttackAlert>());
	alert->Init(alertDirection);
}

void BossUIs::PopStan() {
	stanGaugeUI_->Pop();
}

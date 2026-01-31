#include "TargetUI.h"

void TargetUI::Init() {
	// ----------------------
	// ↓ HPのUI
	// ----------------------

	healthArc_ = std::make_unique<BaseGaugeUI>();
	healthArc_->SetName("healthArc");
	healthArc_->Init("gauge_bg.png", "gauge_front.png");
	healthArc_->GetFront()->Load("TargetUI", "TargetHealthArcFrontUI");
	healthArc_->GetBg()->Load("TargetUI", "TargetHealthArcBgUI");

	healthArc_->SetIsEnable(false);
}

void TargetUI::Update(const Math::Vector2& _reticlePos, BaseEnemy* targetEnemy) {
	if (targetEnemy) {
		const BaseEnemy::BaseParameter& param = targetEnemy->GetBaseParameter();
		const BaseEnemy::BaseParameter& initParam = targetEnemy->GetInitBaseParameter();

		// ----------------------
		// ↓ hpゲージの更新
		// ----------------------
		float fillAmount = param.health / initParam.health;

		healthArc_->SetFillAmount(fillAmount);
		healthArc_->SetPos(_reticlePos);
		healthArc_->Update();
	}
}

void TargetUI::SetIsEnable(bool flag) {
	healthArc_->SetIsEnable(flag);
}
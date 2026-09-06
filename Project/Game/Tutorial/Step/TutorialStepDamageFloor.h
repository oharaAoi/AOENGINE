#pragma once

#include "Game/Tutorial/Step/BaseTutorialStep.h"

/// <summary>
/// ダメージ床を教えるページ。
/// </summary>
class TutorialStepDamageFloor : public BaseTutorialStep {
public:
	TutorialStepDamageFloor() = default;
	~TutorialStepDamageFloor() override = default;

	void Enter(TutorialContext& context) override;
	void Update(TutorialContext& context, float deltaTime) override;
	void Exit(TutorialContext& context) override;

private:

	/// <summary>HPが減っていたら、待ち時間を数えて満タンへ戻す</summary>
	void UpdateHealRecover(TutorialContext& context, float deltaTime);

private:

	// HPが減ってからの経過時間
	float healTimer_ = 0.0f;

	// 回復待ちに入っているか
	bool isWaitingHeal_ = false;

	// HPが減ってから回復させるまでの待ち時間
	static constexpr float kHealWaitTime = 1.0f;

public:// acceccer

	// 入力で送るまで終わらない
	bool IsFinished() const override { return false; }

	const std::string& GetName() const override {
		static const std::string kName = "DamageFloor";
		return kName;
	}
};

#pragma once

#include "Game/Tutorial/Step/BaseTutorialStep.h"

/// <summary>
/// ブロックをつなぐ操作を教えるページ。
/// </summary>
class TutorialStepConnect : public BaseTutorialStep {
public:
	TutorialStepConnect() = default;
	~TutorialStepConnect() override = default;

	void Update(TutorialContext& context, float deltaTime) override;

public:// acceccer

	// 入力で送るまで終わらない
	bool IsFinished() const override { return false; }

	const std::string& GetName() const override {
		static const std::string kName = "Connect";
		return kName;
	}
};

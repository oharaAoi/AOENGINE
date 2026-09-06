#pragma once

#include "Game/Tutorial/Step/BaseTutorialStep.h"

/// <summary>
/// 移動とジャンプを教えるページ。
/// </summary>
class TutorialStepMove : public BaseTutorialStep {
public:
	TutorialStepMove() = default;
	~TutorialStepMove() override = default;

	void Update(TutorialContext& context, float deltaTime) override;

public:// acceccer

	// 入力で送るまで終わらない
	bool IsFinished() const override { return false; }

	const std::string& GetName() const override {
		static const std::string kName = "Move";
		return kName;
	}
};

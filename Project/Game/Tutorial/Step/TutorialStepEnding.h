#pragma once

#include "Game/Tutorial/Step/BaseTutorialStep.h"

/// <summary>
/// 最後のページ。「Xを押したらタイトルへ」を出して待つ。
/// </summary>
class TutorialStepEnding : public BaseTutorialStep {
public:
	TutorialStepEnding() = default;
	~TutorialStepEnding() override = default;

	void Update(TutorialContext& context, float deltaTime) override;

public:// acceccer

	// 入力で送るまで終わらない
	bool IsFinished() const override { return false; }

	const std::string& GetName() const override {
		static const std::string kName = "Ending";
		return kName;
	}

	// ここまで来ていれば全部できている
	bool IsCleared(std::size_t index) const override { (void)index; return true; }
};

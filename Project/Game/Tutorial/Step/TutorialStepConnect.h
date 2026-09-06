#pragma once

#include "Game/Tutorial/Step/BaseTutorialStep.h"

/// <summary>
/// ブロックをつなぐ操作を教えるページ。
/// </summary>
class TutorialStepConnect : public BaseTutorialStep {
public:
	TutorialStepConnect() = default;
	~TutorialStepConnect() override = default;

	void Enter(TutorialContext& context) override;
	void Update(TutorialContext& context, float deltaTime) override;

private:

	// ブロックをつないだか
	bool hasConnected_ = false;

public:

public:// acceccer

	// 入力で送るまで終わらない
	bool IsFinished() const override { return false; }

	const std::string& GetName() const override {
		static const std::string kName = "Connect";
		return kName;
	}

	// 一度でもグループをつないだら達成
	bool IsCleared(std::size_t index) const override { (void)index; return hasConnected_; }
};

#pragma once

#include "Game/Tutorial/Step/BaseTutorialStep.h"

/// <summary>
/// 移動とジャンプを教えるページ。
/// </summary>
class TutorialStepMove : public BaseTutorialStep {
public:
	TutorialStepMove() = default;
	~TutorialStepMove() override = default;

	void Enter(TutorialContext& context) override;
	void Update(TutorialContext& context, float deltaTime) override;

private:

	// 横に動いたか / ジャンプしたか。両方できたらチェックを出す
	bool hasMoved_ = false;
	bool hasJumped_ = false;

	// 動いたとみなす横速度
	static constexpr float kMoveSpeedThreshold = 0.5f;

public:

public:// acceccer

	// 入力で送るまで終わらない
	bool IsFinished() const override { return false; }

	const std::string& GetName() const override {
		static const std::string kName = "Move";
		return kName;
	}

	// 移動用とジャンプ用でチェックを分ける
	std::size_t GetCheckCount() const override { return 2; }

	bool IsCleared(std::size_t index) const override {
		if (index == 0) {
			return hasMoved_;
		}
		return hasJumped_;
	}
};

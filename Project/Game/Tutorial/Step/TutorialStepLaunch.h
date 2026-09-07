#pragma once

#include "Game/Tutorial/Step/BaseTutorialStep.h"

/// <summary>
/// 打ち上げを教えるページ。
/// </summary>
class TutorialStepLaunch : public BaseTutorialStep {
public:
	TutorialStepLaunch() = default;
	~TutorialStepLaunch() override = default;

	void Enter(TutorialContext& context) override;
	void Update(TutorialContext& context, float deltaTime) override;

private:

	// 開始時のボスのHP。これより減っていたら当たったとみなす
	float startBossHp_ = 0.0f;

	bool hasHit_ = false;		// ボスに当てたか
	float waitTimer_ = 0.0f;	// 当ててからの経過時間

	bool isFinished_ = false;	// 終わったか

public:// acceccer

	bool IsFinished() const override { return isFinished_; }

	// ボスに当てるまで進めない
	bool CanSkipByInput() const override { return false; }

	const std::string& GetName() const override {
		static const std::string kName = "Launch";
		return kName;
	}

	// ボスに当てた時点で達成。次のページへ送るのは、この後の待ち時間が過ぎてから
	bool IsCleared(std::size_t index) const override { (void)index; return hasHit_; }
};

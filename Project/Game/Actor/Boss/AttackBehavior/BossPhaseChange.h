#pragma once

#include "Game/Actor/Boss/AttackBehavior/BaseBossAttackBehavior.h"

/// <summary>
/// HPが減ってフェーズが上がった時に割り込む行動
/// </summary>
class BossPhaseChange : public BaseBossAttackBehavior {
public:
	BossPhaseChange() = default;
	~BossPhaseChange() override = default;

	// 入り、更新、抜け
	void Enter(Boss& boss) override;
	void Update(Boss& boss, float deltaTime) override;
	void Exit(Boss& boss) override;

private:

	/// <summary>膨らんで戻る動きを見た目へ反映する</summary>
	void UpdateScalePulse(Boss& boss, float ratio) const;

private:

	// 始まってからの経過時間
	float elapsedTime_ = 0.0f;

	bool isFinished_ = false;	// 終わったか

public:// acceccer

	bool IsFinished() const override { return isFinished_; }

	// 攻撃ではないので、発動のパルスは出さない
	bool IsAttack() const override { return false; }

	const std::string& GetName() const override {
		static const std::string kName = "BossPhaseChange";
		return kName;
	}

	// フェーズ切り替え専用のクリップ。モデル側の綴りに合わせている
	const std::string& GetAnimationName() const override {
		static const std::string kName = "faseChange";
		return kName;
	}
};

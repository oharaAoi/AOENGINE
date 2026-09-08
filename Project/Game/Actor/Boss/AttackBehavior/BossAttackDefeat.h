#pragma once

#include "Game/Actor/Boss/AttackBehavior/BaseBossAttackBehavior.h"
#include "Game/Actor/Boss/BossDefeateAnimation/BossDefeateAnimation.h"

/// <summary>
/// HPが尽きた時の行動
/// </summary>
class BossAttackDefeat : public BaseBossAttackBehavior {
public:
	BossAttackDefeat() = default;
	~BossAttackDefeat() override = default;

	// 入り、更新
	void Enter(Boss& boss) override;
	void Update(Boss& boss, float deltaTime) override;

private:

	// 撃破されてからの経過時間
	float elapsedTime_ = 0.0f;

	// もう姿を消したか
	bool isHidden_ = false;

	std::unique_ptr<BossDefeateAnimation> bossDefateAnimation_;

public:// acceccer

	// 撃破後はここから抜けない。次の行動を選ばせないために常にfalseを返す
	bool IsFinished() const override { return false; }

	// 攻撃ではないので、発動のパルスは出さない
	bool IsAttack() const override { return false; }

	const std::string& GetName() const override {
		static const std::string kName = "BossAttackDefeat";
		return kName;
	}

	const std::string& GetAnimationName() const override {
		static const std::string kName = "damage";
		return kName;
	}
};

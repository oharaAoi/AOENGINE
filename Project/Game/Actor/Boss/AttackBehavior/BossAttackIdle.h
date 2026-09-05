#pragma once

#include "Game/Actor/Boss/AttackBehavior/BaseBossAttackBehavior.h"

/// <summary>
/// 攻撃と攻撃の間の待機。
/// 次の攻撃を始めるまでの時間を min~max の間のランダムで決めて、その間は何もしない
/// </summary>
class BossAttackIdle : public BaseBossAttackBehavior {
public:
	BossAttackIdle() = default;
	~BossAttackIdle() override = default;

	// 入り、更新
	void Enter(Boss& boss) override;
	void Update(Boss& boss, float deltaTime) override;

private:

	// 次の攻撃までの残り時間
	float remainingTime_ = 0.0f;

	bool isFinished_ = false;	// 終わったか

public:// acceccer

	bool IsFinished() const override { return isFinished_; }
	const std::string& GetName() const override {
		static const std::string kName = "BossAttackIdle";
		return kName;
	}

	const std::string& GetAnimationName() const override {
		static const std::string kName = "idle";
		return kName;
	}
};

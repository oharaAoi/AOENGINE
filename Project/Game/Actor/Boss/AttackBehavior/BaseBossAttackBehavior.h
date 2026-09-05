#pragma once
#include<string>
class Boss;
/// <summary>
/// ボスの攻撃行動の基底クラス
/// </summary>
class BaseBossAttackBehavior {
public:
	virtual ~BaseBossAttackBehavior() = default;

	// 行動の開始、終わり
	virtual void Enter(Boss& boss) {}
	virtual void Exit(Boss& boss) {}

	// 更新
	virtual void Update(Boss& boss, float deltaTime) = 0;

	// この行動が終わったかどうか
	virtual bool IsFinished() const = 0;

	// 攻撃名を返す
	virtual const std::string& GetName() const = 0;

	// この行動中に流すアニメーション名を返す。既定は待機
	virtual const std::string& GetAnimationName() const {
		static const std::string kName = "idle";
		return kName;
	}

	// 攻撃までの待ち時間を最初から数え直す
	void ResetStartDelay() { startDelayTimer_ = 0.0f; }

protected:

	/// <summary>
	/// アニメーションを見せてから攻撃を始めるまで待つ
	/// </summary>
	/// <param name="deltaTime">経過時間</param>
	/// <param name="delay">アニメーション再生から攻撃発動までの秒数</param>
	/// <returns>まだ待っている間は true</returns>
	bool WaitStartDelay(float deltaTime, float delay) {
		if (startDelayTimer_ >= delay) {
			return false;
		}

		startDelayTimer_ += deltaTime;
		return startDelayTimer_ < delay;
	}

private:

	// 行動を始めてからの経過時間。攻撃を発動するまでの待ちに使う
	float startDelayTimer_ = 0.0f;

public: // accessor


};

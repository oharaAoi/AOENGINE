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
private:


public: // accessor


};

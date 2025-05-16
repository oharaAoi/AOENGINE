#pragma once
#include <memory>
#include "Game/Actor/Base/BaseAction.h"
// Engine
#include "Engine/System/Input/Input.h"

class Player;

/// <summary>
/// Playerの右手の射撃
/// </summary>
class PlayerActionShotRight :
	public BaseAction<Player> {
public:

	PlayerActionShotRight() = default;
	~PlayerActionShotRight() override {};

	void Build() override;
	void OnStart() override;
	void OnUpdate() override;
	void OnEnd() override;

	void CheckNextAction() override;
	bool IsInput() override;

private:

	void Shot();

private:

	float shotCoolTime_ = 0.2f;	// 弾を放つ時のクールタイム
	float shotTimer_ = 0.0f;

	float notShotTime_ = 1.0f;	// 弾を放っていない時間を計測するよう
	float notShotTimer_ = 0.0f;

	Input* pInput_;

};


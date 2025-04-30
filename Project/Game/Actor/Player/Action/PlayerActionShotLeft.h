#pragma once
#include "Game/Actor/Base/BaseAction.h"
// Engine
#include "Engine/System/Input/Input.h"

class Player;

/// <summary>
/// Playerの左手の攻撃
/// </summary>
class PlayerActionShotLeft :
	public BaseAction<Player> {
public:

	PlayerActionShotLeft() = default;
	~PlayerActionShotLeft() = default;

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


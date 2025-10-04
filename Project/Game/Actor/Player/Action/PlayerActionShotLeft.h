#pragma once
#include <memory>
#include <functional>
// Engine
#include "Engine/System/Input/Input.h"
// Game
#include "Game/Actor/Base/BaseAction.h"
#include "Game/Actor/Weapon/BaseWeapon.h"
#include "Game/Camera/CameraAnimation.h"


class Player;

/// <summary>
/// Playerの左手の攻撃
/// </summary>
class PlayerActionShotLeft :
	public BaseAction<Player> {
public:

	PlayerActionShotLeft() = default;
	~PlayerActionShotLeft()  override {};

	void Build() override;
	void OnStart() override;
	void OnUpdate() override;
	void OnEnd() override;

	void CheckNextAction() override;
	bool IsInput() override;

private:

	void Shot();

	// 準備関数
	void StartUp();

	// 反動
	void Recoil();

private:

	float notShotTime_ = 1.0f;	// 弾を放っていない時間を計測するよう
	float notShotTimer_ = 0.0f;
	bool isFinish_ = true;

	Input* pInput_;
	CameraAnimation* pCameraAnimation_ = nullptr;
	BaseWeapon* pWeapon_ = nullptr;

	std::function<void()> action_;

};


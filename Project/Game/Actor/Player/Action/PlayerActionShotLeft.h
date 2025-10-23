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

public:

	// ビルド処理
	void Build() override;
	// 初期化
	void OnStart() override;
	// 更新
	void OnUpdate() override;
	// 終了処理
	void OnEnd() override;
	// 次のアクションの判定
	void CheckNextAction() override;
	// actionの入力判定
	bool IsInput() override;
	// 編集処理
	void Debug_Gui() override {};

private:

	// 攻撃
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


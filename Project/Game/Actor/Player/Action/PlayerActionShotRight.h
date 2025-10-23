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
	void Debug_Gui() override;

private:

	// 攻撃
	void Shot();

private:

	float notShotTime_ = 1.0f;	// 弾を放っていない時間を計測するよう
	float notShotTimer_ = 0.0f;

	Input* pInput_;

};


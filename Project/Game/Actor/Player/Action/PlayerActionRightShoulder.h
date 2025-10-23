#pragma once
#include "Game/Actor/Base/BaseAction.h"
#include "Game/Actor/Weapon/ShoulderMissile.h"
#include "Engine/System/Input/Input.h"

class Player;

class PlayerActionRightShoulder :
	public BaseAction<Player> {
public:

public:

	PlayerActionRightShoulder() = default;
	~PlayerActionRightShoulder() override {};

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

	Input* pInput_;
	ShoulderMissile* pWeapon_;

	bool isFinish_;
};


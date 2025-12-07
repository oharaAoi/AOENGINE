#pragma once
#include "Game/Actor/Base/BaseAction.h"
#include "Game/Actor/Weapon/LaserRifle.h"
#include "Engine/System/Input/Input.h"

class Player;

/// <summary>
/// Playerの左肩攻撃
/// </summary>
class PlayerActionLeftShoulder :
	public BaseAction<Player> {
public: // コンストラクタ

	PlayerActionLeftShoulder() = default;
	~PlayerActionLeftShoulder() override = default;

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

	AOENGINE::Input* pInput_ = nullptr;
	LaserRifle* pWeapon_ = nullptr;

	bool isFinish_ = false;
};


#pragma once
#include <memory>
#include "Game/Actor/Base/BaseAction.h"

class Player;

/// <summary>
/// 待機アクション
/// </summary>
class PlayerActionIdle :
	public BaseAction<Player>{
public:

	PlayerActionIdle() = default;
	~PlayerActionIdle() override = default;

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
};


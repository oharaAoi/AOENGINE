#pragma once
#include "Game/Actor/Base/BaseAction.h"
#include "Lib/Math/Vector3.h"
#include "Module/Components/WorldTransform.h"

class Player;

/// <summary>
/// Playerが攻撃を受けたときのアクション
/// </summary>
class PlayerActionDamaged :
	public BaseAction<Player> {
public:

	PlayerActionDamaged() = default;
	~PlayerActionDamaged() override = default;


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

private :

	// Parameter -------------------------------
	
	Math::Vector3 velocity_;
	Math::Vector3 acceleration_ = { 0.0f,kGravity, 0.0f };
	WorldTransform* pOwnerTransform_ = nullptr;

};


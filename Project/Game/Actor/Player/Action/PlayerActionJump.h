#pragma once
#include "Game/Actor/Base/BaseAction.h"
#include "Engine/Lib/Math/MyMath.h"
#include "Engine/Components/WorldTransform.h"

// 前方宣言
class Player;

/// <summary>
/// Playerのジャンプ行動
/// </summary>
class PlayerActionJump :
	public BaseAction<Player> {
public:

	PlayerActionJump() = default;
	~PlayerActionJump() = default;

	void Build() override;
	void OnStart() override;
	void OnUpdate() override;
	void OnEnd() override;

	void CheckNextAction() override;
	bool IsInput() override;

private:	// action

	/// <summary>
	/// main action
	/// </summary>
	void Jump();

	/// <summary>
	/// 重力の適用
	/// </summary>
	void ApplyGravity();

private:	// variable

	Vector3 velocity_;
	Vector3 acceleration_ = { 0.0f,kGravity, 0.0f };

	WorldTransform* pOwnerTransform_ = nullptr;

};


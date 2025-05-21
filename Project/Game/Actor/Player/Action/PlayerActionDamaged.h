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

	void Build() override;
	void OnStart() override;
	void OnUpdate() override;
	void OnEnd() override;

	void CheckNextAction() override;
	bool IsInput() override;

	void ApplyGravity();

private :

	// Parameter -------------------------------
	float actionTimer_;

	Vector3 velocity_;
	Vector3 acceleration_ = { 0.0f,kGravity, 0.0f };
	WorldTransform* pOwnerTransform_ = nullptr;

};


#pragma once
#include "Game/Actor/Base/BaseAction.h"

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


private :

	// Parameter -------------------------------
	float actionTimer_;

};


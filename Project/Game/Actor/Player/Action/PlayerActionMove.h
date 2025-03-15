#pragma once
#include "Game/Actor/Base/BaseAction.h"

class Player;

class PlayerActionMove :
	public BaseAction<Player>{
public:

	PlayerActionMove() = default;
	~PlayerActionMove() = default;

	void Build() override;
	void OnStart() override;
	void OnUpdate() override;
	void OnEnd() override;

	void CheckNextAction() override;
	bool IsInput() override;

private:

	const float kDeadZone_ = 0.1f;

};


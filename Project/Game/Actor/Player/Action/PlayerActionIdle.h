#pragma once
#include <memory>
#include "Game/Actor/Base/BaseAction.h"

class Player;

class PlayerActionIdle :
	public BaseAction<Player>{
public:

	PlayerActionIdle() = default;
	~PlayerActionIdle() override = default;

	void Build() override;
	void OnStart() override;
	void OnUpdate() override;
	void OnEnd() override;

	void CheckNextAction() override;
	bool IsInput() override;

private:

	// Parameter -------------------------------
	float actionTimer_;

};


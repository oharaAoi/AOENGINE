#pragma once
#include <memory>
#include "Game/Actor/Base/BaseAction.h"

class Player;

class PlayerActionIdle :
	public BaseAction<Player>{
public:

	PlayerActionIdle() = default;
	~PlayerActionIdle() = default;

	void Build() override;
	void OnStart() override;
	void OnUpdate() override;
	void OnEnd() override;

	void CheckNextAction() override;
	bool IsInput() override;

private:

	// NextAction -------------------------------
	std::shared_ptr<BaseAction<Player>> moveAction_;

};


#pragma once
#include <Game/State/ICharacterState.h>

class Player;

class PlayerIdleState :
	public ICharacterState<Player>{
public:

	PlayerIdleState() = default;
	~PlayerIdleState() = default;

	void OnStart() override;
	void OnUpdate() override;
	void OnExit() override;

};


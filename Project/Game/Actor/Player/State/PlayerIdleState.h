#pragma once
#include <Game/State/ICharacterState.h>
#include "Engine/Module/Components/Animation/VectorTween.h"

class Player;

class PlayerIdleState :
	public ICharacterState<Player>{
public:

	PlayerIdleState() = default;
	~PlayerIdleState() = default;

	void OnStart() override;
	void OnUpdate() override;
	void OnExit() override;

	void Debug_Gui() override {};

private:

	float floatingValue_;
	VectorTween<float> floatingTween_;

};


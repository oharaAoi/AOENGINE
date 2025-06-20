#pragma once
#include <Game/State/ICharacterState.h>
#include "Engine/Module/Components/Animation/VectorTween.h"

class Boss;

class BossIdleState :
	public ICharacterState<Boss> {
public:

	BossIdleState() = default;
	~BossIdleState() = default;

	void OnStart() override;
	void OnUpdate() override;
	void OnExit() override;

#ifdef _DEBUG
	void Debug_Gui() override {};
#endif

private:

	float floatingValue_;
	VectorTween<float> floatingTween_;

};


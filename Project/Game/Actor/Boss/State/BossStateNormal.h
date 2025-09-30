#pragma once
#include "Game/State/ICharacterState.h"
#include "Engine/Module/Components/Animation/VectorTween.h"

class Boss;

/// <summary>
/// 通常状態
/// </summary>
class BossStateNormal :
	public ICharacterState<Boss> {
public:

	BossStateNormal() = default;
	~BossStateNormal() = default;

	void OnStart() override;
	void OnUpdate() override;
	void OnExit() override;

	void Debug_Gui() override;

public:

	VectorTween<float> floatingTween_;

};


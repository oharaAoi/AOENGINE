#pragma once
#include "Engine/System/AI/State/IWorldState.h"

/// <summary>
/// Bossの状態を所有しているクラス
/// </summary>
class BossWorldState :
	public IWorldState {
public:

	BossWorldState() = default;
	~BossWorldState() override = default;

};


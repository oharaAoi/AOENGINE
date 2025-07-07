#pragma once
#include "Engine/Lib/Math/Quaternion.h"
#include "Engine/Module/Components/AI/IBehaviorNode.h"
#include "Game/Actor/Base/BaseAction.h"

class Boss;

class BossActionIdle :
	public BaseAction<Boss> {
public:

	BossActionIdle() = default;
	~BossActionIdle() override = default;

	void Build() override;
	void OnStart() override;
	void OnUpdate() override;
	void OnEnd() override;

	void CheckNextAction() override;
	bool IsInput() override;

	void Debug_Gui() override;

private:

	Quaternion playerToRotate_;

};


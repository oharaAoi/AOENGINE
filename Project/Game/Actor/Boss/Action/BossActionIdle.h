#pragma once
#include "Engine/Lib/Math/Quaternion.h"
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

#ifdef _DEBUG
	void Debug_Gui() override;
#endif // _DEBUG


private:

	float actionTimer_;

	Quaternion playerToRotate_;

};


#pragma once
#include "Engine/Module/Components/AI/ITaskNode.h"

class Boss;

class BossActionWait :
	public ITaskNode<Boss> {
public:

	BossActionWait() = default;
	~BossActionWait() override = default;

	BehaviorStatus Execute() override;

	void Debug_Gui() override;

	void Init() override;
	void Update() override;
	void End() override;

private:

	float actionTimer_;

};


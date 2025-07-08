#pragma once
#include "Engine/Module/Components/AI/ITaskNode.h"

class Boss;


class BossActionFloat :
	public ITaskNode<Boss>{

public:

	BossActionFloat() = default;
	~BossActionFloat() override = default;

	BehaviorStatus Execute() override;

	void Init() override;
	void Update() override;
	void End() override;

private:

	float actionTimer_;
};


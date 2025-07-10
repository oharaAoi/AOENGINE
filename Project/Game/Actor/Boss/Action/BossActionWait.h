#pragma once
#include <functional>
#include "Engine/Module/Components/AI/ITaskNode.h"

class Boss;

class BossActionWait :
	public ITaskNode<Boss> {
public:

	BossActionWait() = default;
	~BossActionWait() override = default;

	std::shared_ptr<IBehaviorNode> Clone() const override {
		return std::make_shared<BossActionWait>(*this);
	}

	BehaviorStatus Execute() override;

	void Debug_Gui() override;

	bool IsFinish() override;
	bool CanExecute() override;

	void Init() override;
	void Update() override;
	void End() override;

private:

};


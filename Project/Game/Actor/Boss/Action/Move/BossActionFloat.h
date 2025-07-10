#pragma once
#include "Engine/Module/Components/AI/ITaskNode.h"

class Boss;


class BossActionFloat :
	public ITaskNode<Boss>{

public:

	BossActionFloat() = default;
	~BossActionFloat() override = default;

	std::shared_ptr<IBehaviorNode> Clone() const override {
		return std::make_shared<BossActionFloat>(*this);
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


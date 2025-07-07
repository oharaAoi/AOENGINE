#pragma once
#include "Engine/Module/Components/AI/IBehaviorNode.h"
#include "Engine/Module/Components/Attribute/AttributeGui.h"
#include <memory>

template<typename OwnerType>
class ITaskNode :
	public IBehaviorNode,
	public AttributeGui {
public:

	ITaskNode() = default;
	virtual ~ITaskNode() override = default;

	virtual BehaviorStatus Execute() override = 0;

	virtual void Init() = 0;
	virtual void Update() = 0;
	virtual void End() = 0;

	void SetTarget(OwnerType* owner) { pTarget_ = owner; }

	void Debug_Gui() override {};

private:

	OwnerType* pTarget_ = nullptr;


};


#pragma once
#include "Engine/Module/Components/AI/IBehaviorNode.h"
#include <memory>

template<typename OwnerType>
class ITaskNode :
	public IBehaviorNode {
public:

	ITaskNode();
	virtual ~ITaskNode() override = default;

	virtual BehaviorStatus Execute() override = 0;

	virtual void Init() = 0;
	virtual void Update() = 0;
	virtual void End() = 0;

	void SetTarget(OwnerType* owner) { pTarget_ = owner; }

	virtual void Debug_Gui() override {};

protected:

	OwnerType* pTarget_ = nullptr;

};

template<typename OwnerType>
inline ITaskNode<OwnerType>::ITaskNode() {
	type_ = NodeType::Task;
	color_ = ImColor(153, 102, 204);
	baseColor_ = color_;
	isLeafNode_ = true;
}

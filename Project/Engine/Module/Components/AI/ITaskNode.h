#pragma once
#include <memory>
#include <functional>
#include "Engine/Module/Components/AI/IBehaviorNode.h"
#include "Engine/Lib/GameTimer.h"

template<typename OwnerType>
class ITaskNode :
	public IBehaviorNode {
public:

	ITaskNode();
	virtual ~ITaskNode() override = default;

	virtual std::shared_ptr<IBehaviorNode> Clone() const override = 0;

	virtual BehaviorStatus Execute() override = 0;

	float EvaluateWeight() override = 0;

	virtual void Init() = 0;
	virtual void Update() = 0;
	virtual void End() = 0;

	virtual bool IsFinish() = 0;
	virtual bool CanExecute() = 0;

	virtual void Debug_Gui() override;

	virtual BehaviorStatus Action();

	void SetTarget(OwnerType* owner) { pTarget_ = owner; }

protected:

	OwnerType* pTarget_ = nullptr;

	std::function<void()> action_;

	float taskTimer_;
};

template<typename OwnerType>
inline ITaskNode<OwnerType>::ITaskNode() {
	type_ = NodeType::Task;
	color_ = ImColor(153, 102, 204);
	baseColor_ = color_;
	isLeafNode_ = true;
}

template<typename OwnerType>
inline void ITaskNode<OwnerType>::Debug_Gui() {
	ImGui::BulletText("Task Name : %s", node_.name.c_str());

}

template<typename OwnerType>
inline BehaviorStatus ITaskNode<OwnerType>::Action() {
	if (coolTime_ > 0.0f) {
		return BehaviorStatus::Failure;
	}

	// 非アクティブ状態なら初期化を行う
	if (state_ == BehaviorStatus::Inactive) {
		if (!CanExecute()) {
			return BehaviorStatus::Failure;
		}
		taskTimer_ = 0;
		state_ = BehaviorStatus::Running;
		Init();
	}

	// 更新
	Update();

	// 終了フラグがtrueなら終了処理を行う
	if (IsFinish()) {
		End();
		state_ = BehaviorStatus::Inactive;
		return BehaviorStatus::Success;
	}
	return BehaviorStatus::Running;
}

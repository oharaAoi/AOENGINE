#pragma once
#include <memory>
#include <functional>
#include "Engine/Module/Components/AI/IBehaviorNode.h"
#include "Engine/Module/Components/AI/UtilityAI/UtilityEvaluator.h"
#include "Engine/Lib/GameTimer.h"
#include "Engine/Utilities/Timer.h"

/// <summary>
/// nodeのtask
/// </summary>
/// <typeparam name="OwnerType"></typeparam>
template<typename OwnerType>
class ITaskNode :
	public IBehaviorNode {
public: // コンストラクタ

	ITaskNode();
	virtual ~ITaskNode() override = default;

	virtual std::shared_ptr<IBehaviorNode> Clone() const override = 0;

public:

	/// <summary>
	/// jsonへ変換
	/// </summary>
	/// <returns></returns>
	json ToJson() override;

	/// <summary>
	/// jsonから適応させる
	/// </summary>
	/// <param name="_jsonData"></param>
	void FromJson(const json& _jsonData) override;

	/// <summary>
	/// 実行処理
	/// </summary>
	/// <returns></returns>
	virtual BehaviorStatus Execute() override = 0;

	/// <summary>
	/// weightを算出
	/// </summary>
	/// <returns></returns>
	float EvaluateWeight() override = 0;

	// 初期化処理
	virtual void Init() = 0;
	// 更新処理
	virtual void Update() = 0;
	// 終了処理
	virtual void End() = 0;
	// 終了確認
	virtual bool IsFinish() = 0;
	// 実行できるを判別
	virtual bool CanExecute() = 0;
	// 編集処理
	virtual void Debug_Gui() override;
	// 行動を行う
	virtual BehaviorStatus Action();

public:

	void SetTarget(OwnerType* owner) { pTarget_ = owner; }

private:

	// 待機処理
	bool Wait();

protected:

	OwnerType* pTarget_ = nullptr;

	std::function<void()> action_;

	float taskTimer_ = 0.0f;

	Timer waitTimer_;
	Timer coolTimer_;

	UtilityEvaluator evaluator_;

};

template<typename OwnerType>
inline ITaskNode<OwnerType>::ITaskNode() {
	type_ = NodeType::Task;
	color_ = ImColor(153, 102, 204);
	baseColor_ = color_;
	isLeafNode_ = true;
	waitTimer_.targetTime_ = 1;
	evaluator_ = UtilityEvaluator();
}

template<typename OwnerType>
inline json ITaskNode<OwnerType>::ToJson() {
	json item;
	item["name"] = node_.name;
	item["nodeType"] = static_cast<int>(type_);
	item["nodePos"] = json{ {"x", pos_.x}, {"y", pos_.y} };
	item["waitTime"] = waitTimer_.targetTime_;
	item["coolTime"] = coolTimer_.targetTime_;
	item["children"] = json::array();
	
	for (const auto& child : children_) {
		item["children"].push_back(child->ToJson());
	}
	return item;
}

template<typename OwnerType>
inline void ITaskNode<OwnerType>::FromJson(const json& _jsonData) {
	node_.name = _jsonData["name"];
	type_ = _jsonData["nodeType"];
	pos_ = Vector2(_jsonData["nodePos"]["x"], _jsonData["nodePos"]["y"]);

	if (_jsonData.contains("waitTIme")) {
		waitTimer_.targetTime_ = _jsonData["waitTime"].get<float>();
	}
	if (_jsonData.contains("coolTime")) {
		coolTimer_.targetTime_ = _jsonData["coolTime"].get<float>();
	}
}

template<typename OwnerType>
inline void ITaskNode<OwnerType>::Debug_Gui() {
	ImGui::BulletText("Task Name : %s", node_.name.c_str());
	ImGui::SliderFloat("wait_timer", &waitTimer_.timer_, 0.0f, waitTimer_.targetTime_);
	ImGui::DragFloat("waitTime", &waitTimer_.targetTime_, 0.1f);
	ImGui::SliderFloat("cool_timer", &coolTimer_.timer_, 0.0f, coolTimer_.targetTime_);
	ImGui::DragFloat("coolTime", &coolTimer_.targetTime_, 0.1f);
	evaluator_.Debug_Gui();
}

template<typename OwnerType>
inline BehaviorStatus ITaskNode<OwnerType>::Action() {
	if (coolTimer_.Run(GameTimer::DeltaTime())) {
		return BehaviorStatus::Failure;
	}

	// 非アクティブ状態なら初期化を行う
	if (state_ == BehaviorStatus::Inactive) {
		if (!CanExecute()) {
			return BehaviorStatus::Failure;
		}
		taskTimer_ = 0;
		waitTimer_.timer_ = 0;
		state_ = BehaviorStatus::Running;
		Init();
	}

	// 更新
	if (!IsFinish()) {
		Update();
	} else {
		if (Wait()) {
			End();
			coolTimer_.timer_ = 0;
			state_ = BehaviorStatus::Inactive;
			return BehaviorStatus::Success;
		}
	}

	return BehaviorStatus::Running;
}

template<typename OwnerType>
inline bool ITaskNode<OwnerType>::Wait() {
	if (!waitTimer_.Run(GameTimer::DeltaTime())) {
		return true;
	}
	return false;
}

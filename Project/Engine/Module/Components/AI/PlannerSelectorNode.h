#pragma once
#include "Engine/Module/Components/AI/IBehaviorNode.h"
#include "Engine/Lib/Json/IJsonConverter.h"

class PlannerSelectorNode :
	public IBehaviorNode {
public: // コンストラクタ

	PlannerSelectorNode();
	~PlannerSelectorNode() override = default;

	std::shared_ptr<IBehaviorNode> Clone() const override {
		return std::make_shared<PlannerSelectorNode>(*this);
	}

public:

	// jsonへ
	json ToJson() override;

	// 実行
	BehaviorStatus Execute() override;

	// ウェイト値の算出
	float EvaluateWeight() override;

	// 編集処理
	void Debug_Gui() override;

private:

	/// <summary>
	/// 評価値を表示する
	/// </summary>
	void PriorityDisplay();

	/// <summary>
	/// 評価値を外部に出す
	/// </summary>
	void PriorityOutput();

private:

	std::unordered_map<uint32_t, float> priorityMap_;
	bool reset_ = false;

};


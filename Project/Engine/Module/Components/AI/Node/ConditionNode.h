#pragma once
#include "Engine/Module/Components/AI/Node/IBehaviorNode.h"
#include "Engine/Module/Components/AI/State/WorldStateValue.h"

/// <summary>
/// if文の代わりとなるNode
/// </summary>
class ConditionNode :
	public IBehaviorNode {
public: // コンストラクタ

	ConditionNode();
	~ConditionNode() override = default;

	std::shared_ptr<IBehaviorNode> Clone() const override {
		return std::make_shared<ConditionNode>(*this);
	}

public:

	/// <summary>
	/// jsonへ
	/// </summary>
	/// <returns></returns>
	json ToJson() override;

	/// <summary>
	/// jsonから保存項目を適応
	/// </summary>
	/// <param name="_jsonData"></param>
	void FromJson(const json& _jsonData) override;

	// 実行
	BehaviorStatus Execute() override;

	// ウェイト値の算出
	float EvaluateWeight() override;

	// 編集処理
	void Debug_Gui() override;

private:

	/// <summary>
	/// 判断する
	/// </summary>
	bool Compare(const WorldStateValue& lhs,
				 const WorldStateValue& rhs,
				 const std::string& op);

private:

	static const int32_t kOperatorCount_ = 6;
	const char* conditionOps[kOperatorCount_] = {
	"==", "!=", ">", "<", ">=", "<="
	};
	int32_t opIndex_ = 0;
	int32_t leftKeyIndex_ = 0;
	int32_t rightKeyIndex_ = 0;

	std::string leftKey_ = "";
	std::string rightKey_ = "";

	float freeValue_ = 0;

	int32_t radioButtonIndex_ = 0;

	WorldStateValue value_;

};
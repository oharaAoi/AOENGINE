#pragma once
#include "Engine/System/AI/Node/BaseBehaviorNode.h"

namespace AI {

/// <summary>
/// タスクを同時に実行するNode
/// </summary>
class ParallelNode :
	public BaseBehaviorNode {
public: // コンストラクタ

	ParallelNode();
	~ParallelNode() override = default;

	std::shared_ptr<BaseBehaviorNode> Clone() const override {
		return std::make_shared<ParallelNode>(*this);
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

	// 実行中のNodeの名前を取得する
	std::string RunNodeName() override;
};

}
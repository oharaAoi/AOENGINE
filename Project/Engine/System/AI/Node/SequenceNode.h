#pragma once
#include "Engine/System/AI/Node/IBehaviorNode.h"

/// <summary>
/// 一番左の子が成功したら次の子へと遷移するNode
/// </summary>
class SequenceNode :
	public IBehaviorNode {
public: // コンストラクタ

	SequenceNode();
	~SequenceNode() override = default;

	std::shared_ptr<IBehaviorNode> Clone() const override {
		return std::make_shared<SequenceNode>(*this);
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


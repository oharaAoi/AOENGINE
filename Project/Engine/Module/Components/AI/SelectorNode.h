#pragma once
#include "Engine/Module/Components/AI/IBehaviorNode.h"

/// <summary>
/// 左から実行をし、 失敗したら次へ成功したら親へ戻るNode
/// </summary>
class SelectorNode :
	public IBehaviorNode {
public: // コンストラクタ

	SelectorNode();
	~SelectorNode() override = default;

	std::shared_ptr<IBehaviorNode> Clone() const override {
		return std::make_shared<SelectorNode>(*this);
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
};
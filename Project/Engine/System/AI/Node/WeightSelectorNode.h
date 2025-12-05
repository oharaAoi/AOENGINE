#pragma once
#include <unordered_map>
#include "Engine/System/AI/Node/BaseBehaviorNode.h"

/// <summary>
/// 重みの計算を行ってから選択をするセレクタ
/// </summary>
class WeightSelectorNode :
	public BaseBehaviorNode {
public: // コンストラクタ

	WeightSelectorNode();
	~WeightSelectorNode() override = default;

	std::shared_ptr<BaseBehaviorNode> Clone() const override {
		return std::make_shared<WeightSelectorNode>(*this);
	}

public:

	// jsonへ
	json ToJson() override;

	// 実行
	BehaviorStatus Execute() override;

	// ウェイト値の算出
	float EvaluateWeight() override { return 0; }

	// 編集処理
	void Debug_Gui() override;

	// 実行中のNodeの名前を取得する
	std::string RunNodeName() override;

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

	std::unordered_map<uint32_t, float> weightMap_;
	bool isReset_ = false;

};


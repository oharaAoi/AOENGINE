#pragma once
#include "Engine/System/AI/Node/BaseBehaviorNode.h"
#include <cstdint>

namespace AI {

/// <summary>
/// タスクを同時に実行するNode
/// </summary>
class ParallelNode :
	public BaseBehaviorNode {
public: // コンストラクタ

	ParallelNode();
	~ParallelNode() override = default;

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

	/// <summary>
	/// 重みのテーブルに表示する項目
	/// </summary>
	void WeightTableItem() override {};

	/// <summary>
	/// taskの初期化用意
	/// </summary>
	void ResetNode() override;

private:

	uint32_t successCount_ = 0;

};

}
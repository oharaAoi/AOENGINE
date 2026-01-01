#pragma once
#include "Engine/System/AI/Node/BaseBehaviorNode.h"

namespace AI {

/// <summary>
/// 左から実行をし、 失敗したら次へ成功したら親へ戻るNode
/// </summary>
class SelectorNode :
	public BaseBehaviorNode {
public: // コンストラクタ

	SelectorNode();
	~SelectorNode() override = default;

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
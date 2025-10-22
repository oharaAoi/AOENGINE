#pragma once
#include "Engine/Module/Components/AI/IBehaviorNode.h"
#include <memory>

/// <summary>
/// Treeの根本となるクラス
/// </summary>
class BehaviorRootNode :
	public IBehaviorNode {
public: //コンストラクタ

	BehaviorRootNode();
	~BehaviorRootNode() = default;

	std::shared_ptr<IBehaviorNode> Clone() const override {
		return std::make_shared<BehaviorRootNode>(*this);
	}

public:

	/// <summary>
	/// jsonに変換
	/// </summary>
	/// <returns></returns>
	json ToJson() override;

	/// <summary>
	/// 実行する
	/// </summary>
	/// <returns></returns>
	BehaviorStatus Execute() override;

	/// <summary>
	/// ウェイト値の計算
	/// </summary>
	/// <returns></returns>
	float EvaluateWeight() override { return 0; }

	/// <summary>
	/// 編集処理
	/// </summary>
	void Debug_Gui() override;

private:



};


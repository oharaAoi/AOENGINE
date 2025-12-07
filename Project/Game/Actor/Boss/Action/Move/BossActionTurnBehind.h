#pragma once
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/System/AI/Node/BaseTaskNode.h"

class Boss;

/// <summary>
/// 敵の背後に回る処理
/// </summary>
class BossActionTurnBehind :
	public AI::BaseTaskNode<Boss> {
public: // データ構造体


public: // コンストラクタ

	BossActionTurnBehind() = default;
	~BossActionTurnBehind() = default;

	std::shared_ptr<AI::BaseBehaviorNode> Clone() const override {
		return std::make_shared<BossActionTurnBehind>(*this);
	}

public:

	// 実行処理
	BehaviorStatus Execute() override;
	// weight値の計算
	float EvaluateWeight() override;
	// 編集処理
	void Debug_Gui() override;
	// 終了判定
	bool IsFinish() override;
	// Actionを実行できるかの確認
	bool CanExecute() override;
	// 初期化
	void Init() override;
	// 更新
	void Update() override;
	// 終了処理
	void End() override;

};


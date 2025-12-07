#pragma once
#include <functional>
#include "Engine/System/AI/Node/BaseTaskNode.h"

class Boss;

class BossActionWait :
	public AI::BaseTaskNode<Boss> {
public:

	BossActionWait() = default;
	~BossActionWait() override = default;

	std::shared_ptr<AI::BaseBehaviorNode> Clone() const override {
		return std::make_shared<BossActionWait>(*this);
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


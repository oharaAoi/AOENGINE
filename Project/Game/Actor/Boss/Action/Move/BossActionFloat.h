#pragma once
#include "Engine/System/AI/Node/BaseTaskNode.h"
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/Lib/Math/Curve.h"

class Boss;


class BossActionFloat :
	public AI::BaseTaskNode<Boss>{

public:

	struct Parameter : public AOENGINE::IJsonConverter {
		float moveTime = 0.5f;		// 移動時間
		float moveSpeed = 10.0f;	// 移動速度
		Math::Curve moveCurve;

		Parameter() { SetName("BossActionFloat"); }

		json ToJson(const std::string& id) const override {
			json curveJson = moveCurve.ToJson();
			return AOENGINE::JsonBuilder(id)
				.Add("moveTime", moveTime)
				.Add("moveSpeed", moveSpeed)
				.Add("moveCurve", curveJson)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			Convert::fromJson(jsonData, "moveTime", moveTime);
			Convert::fromJson(jsonData, "moveSpeed", moveSpeed);
			Convert::fromJson(jsonData, "moveCurve", moveCurve);
		}

		void Debug_Gui() override;
	};

public:

	BossActionFloat() = default;
	~BossActionFloat() override = default;

	std::shared_ptr<AI::BaseBehaviorNode> Clone() const override {
		return std::make_shared<BossActionFloat>(*this);
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

private:
	Parameter param_;
};


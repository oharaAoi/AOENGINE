#pragma once
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/System/AI/Node/BaseTaskNode.h"

class Boss;

/// <summary>
/// ボスが近づきながら火炎放射で攻撃を行なってくる行動
/// </summary>
class BossActionApproachFlamethrower :
	public AI::BaseTaskNode<Boss> {
public:

	struct Parameter : public AOENGINE::IJsonConverter {
		float moveSpeed = 20.f;			// 移動速度
		float moveTime = 2.0f;			// 移動時間
		float minDecel = 1.0f;			// 最小の減速率
		float maxDecel = 12.0f;			// 最大の減速率
		float stopThreshold = 12.0f;	// 止まる速度
		float lookTime = 2.0f;
		float backLength = 2.0f;
		Math::Curve decelCurve;

		Parameter() {
			SetGroupName("BossAction");
			SetName("bossActionApproach");
		}

		json ToJson(const std::string& id) const override {
			json curveJson = decelCurve.ToJson();
			return AOENGINE::JsonBuilder(id)
				.Add("moveSpeed", moveSpeed)
				.Add("moveTime", moveTime)
				.Add("minDecel", minDecel)
				.Add("maxDecel", maxDecel)
				.Add("stopThreshold", stopThreshold)
				.Add("lookTime", lookTime)
				.Add("backLength", backLength)
				.Add("decelCurve", curveJson)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			Convert::fromJson(jsonData, "moveSpeed", moveSpeed);
			Convert::fromJson(jsonData, "moveTime", moveTime);
			Convert::fromJson(jsonData, "minDecel", minDecel);
			Convert::fromJson(jsonData, "maxDecel", maxDecel);
			Convert::fromJson(jsonData, "stopThreshold", stopThreshold);
			Convert::fromJson(jsonData, "lookTime", lookTime);
			Convert::fromJson(jsonData, "backLength", backLength);
			decelCurve.FromJson(jsonData, "decelCurve");
		}

		void Debug_Gui() override;
	};

public:

	BossActionApproachFlamethrower();
	~BossActionApproachFlamethrower() override = default;

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

	// 敵に近づいて来る
	void Approach();

private:

	Parameter param_;

	Math::Vector3 direction_;
	Math::Vector3 velocity_;

};


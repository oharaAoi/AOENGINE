#pragma once
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/Lib/Math/Curve.h"
#include "Engine/Module/Components/AI/ITaskNode.h"

class Boss;

/// <summary>
/// 小刻みに横に動く移動
/// </summary>
class BossActionStrafe :
	public ITaskNode<Boss> {
public:

	struct Parameter : public IJsonConverter {
		float moveSpeed = 60.f;
		float moveTime = 0.5f;
		float getDistance = 10.0f;
		float decayRate = 4.0f;
		Curve curve;

		Parameter() { SetName("bossStrafe"); }

		json ToJson(const std::string& id) const override {
			json curveData = curve.ToJson();
			return JsonBuilder(id)
				.Add("moveSpeed", moveSpeed)
				.Add("moveTime", moveTime)
				.Add("getDistance", getDistance)
				.Add("decayRate", decayRate)
				.Add("curveData", curveData)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			fromJson(jsonData, "moveSpeed", moveSpeed);
			fromJson(jsonData, "moveTime", moveTime);
			fromJson(jsonData, "getDistance", getDistance);
			fromJson(jsonData, "decayRate", decayRate);
			curve.FromJson(jsonData, "curveData");
		}

		void Debug_Gui() override;
	};

public:

	BossActionStrafe() = default;
	~BossActionStrafe() = default;

	std::shared_ptr<IBehaviorNode> Clone() const override {
		return std::make_shared<BossActionStrafe>(*this);
	}

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

	// 旋回処理
	void Spin();

	// 止まる処理
	void Stop();

private:

	Parameter param_;

	Vector3 velocity_;
	Vector3 accel_;

	bool stopping_;
};


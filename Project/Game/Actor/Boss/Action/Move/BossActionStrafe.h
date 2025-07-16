#pragma once
#include "Engine/Lib/Json/IJsonConverter.h"
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

		Parameter() { SetName("bossStrafe"); }

		json ToJson(const std::string& id) const override {
			return JsonBuilder(id)
				.Add("moveSpeed", moveSpeed)
				.Add("moveTime", moveTime)
				.Add("getDistance", getDistance)
				.Add("decayRate", decayRate)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			fromJson(jsonData, "moveSpeed", moveSpeed);
			fromJson(jsonData, "moveTime", moveTime);
			fromJson(jsonData, "getDistance", getDistance);
			fromJson(jsonData, "decayRate", decayRate);
		}
	};

public:

	BossActionStrafe() = default;
	~BossActionStrafe() = default;

	std::shared_ptr<IBehaviorNode> Clone() const override {
		return std::make_shared<BossActionStrafe>(*this);
	}

	BehaviorStatus Execute() override;

	float EvaluateWeight() override;

	void Debug_Gui() override;

	bool IsFinish() override;
	bool CanExecute() override;

	void Init() override;
	void Update() override;
	void End() override;

private:

	void Spin();

	void Stop();

private:

	Parameter param_;

	Vector3 velocity_;
	Vector3 accel_;

	bool stopping_;
};


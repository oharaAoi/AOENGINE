#pragma once
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Game/Actor/Base/BaseAction.h"

class Boss;

/// <summary>
/// 小刻みに横に動く移動
/// </summary>
class BossActionStrafe :
	public BaseAction<Boss> {
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

	void Build() override;
	void OnStart() override;
	void OnUpdate() override;
	void OnEnd() override;

	void CheckNextAction() override;
	bool IsInput() override;

	void Debug_Gui() override;

private:

	void Spin();

	void Stop();

private:

	Parameter param_;

	Vector3 velocity_;
	Vector3 accel_;

	bool stopping_;
};


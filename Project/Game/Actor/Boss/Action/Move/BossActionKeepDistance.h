#pragma once
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/Module/Components/AI/ITaskNode.h"

class Boss;

/// <summary>
/// Bossの距離を取る行動
/// </summary>
class BossActionKeepDistance :
	public ITaskNode<Boss> {
public:

	struct Parameter : public IJsonConverter {
		float moveSpeed = 60.f;
		float moveTime = 2.0f;
		float getDistance = 10.0f;
		float decayRate = 4.0f;

		Parameter() { SetName("bossKeepDistance"); }

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

	BossActionKeepDistance() = default;
	~BossActionKeepDistance() = default;

	BehaviorStatus Execute() override;

	void Debug_Gui() override;

	bool IsFinish() override;
	bool CanExecute() override;

	void Init() override;
	void Update() override;
	void End() override;

public:

	void Direct();

	void Spin();

	void Stop();

private:

	Parameter param_;

	Vector3 velocity_;
	Vector3 accel_;

	int moveType_;

	// 旋回に必要な座標
	Vector3 centerPos_;

	bool stopping_;

};


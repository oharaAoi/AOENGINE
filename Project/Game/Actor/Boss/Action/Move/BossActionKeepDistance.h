#pragma once
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Game/Actor/Base/BaseAction.h"

class Boss;

/// <summary>
/// Bossの距離を取る行動
/// </summary>
class BossActionKeepDistance :
	public BaseAction<Boss> {
public:

	struct Parameter : public IJsonConverter {
		float moveSpeed = 60.f;
		float moveTime = 2.0f;
		float getDistance = 10.0f;

		Parameter() { SetName("bossKeepDistance"); }

		json ToJson(const std::string& id) const override {
			return JsonBuilder(id)
				.Add("moveSpeed", moveSpeed)
				.Add("moveTime", moveTime)
				.Add("getDistance", getDistance)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			fromJson(jsonData, "moveSpeed", moveSpeed);
			fromJson(jsonData, "moveTime", moveTime);
			fromJson(jsonData, "getDistance", getDistance);
		}
	};

public:

	BossActionKeepDistance() = default;
	~BossActionKeepDistance() = default;

	void Build() override;
	void OnStart() override;
	void OnUpdate() override;
	void OnEnd() override;

	void CheckNextAction() override;
	bool IsInput() override;

	void Debug_Gui() override;

public:

	void Direct();

	void Spin();

private:

	Parameter param_;

	Vector3 velocity_;
	Vector3 accel_;

	int moveType_;

	// 旋回に必要な座標
	Vector3 centerPos_;

};


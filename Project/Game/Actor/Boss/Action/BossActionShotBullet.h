#pragma once
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Game/Actor/Base/BaseAction.h"

class Boss;

/// <summary>
/// 通常弾を撃つ
/// </summary>
class BossActionShotBullet :
	public BaseAction<Boss> {
public:

	struct Parameter : public IJsonConverter {
		float shotInterval = 0.1f;
		float bulletSpeed = 80.0f;
		int kFireCount = 20;

		Parameter() { SetName("bossActionShotBullet"); }
		
		json ToJson(const std::string& id) const override {
			return JsonBuilder(id)
				.Add("shotInterval", shotInterval)
				.Add("bulletSpeed", bulletSpeed)
				.Add("kFireCount", kFireCount)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			fromJson(jsonData, "shotInterval", shotInterval);
			fromJson(jsonData, "bulletSpeed", bulletSpeed);
			fromJson(jsonData, "kFireCount", kFireCount);
		}
	};
public:

	BossActionShotBullet() = default;
	~BossActionShotBullet() override = default;

	void Build() override;
	void OnStart() override;
	void OnUpdate() override;
	void OnEnd() override;

	void CheckNextAction() override;
	bool IsInput() override;

	void Debug_Gui() override;

private:

	void Shot();

private:

	Parameter param_;

	bool isFinishShot_;
	int fireCount_;

};


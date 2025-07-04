#pragma once
#include <memory>
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Game/Actor/Base/BaseAction.h"
#include "Game/Actor/Boss/Action/Attack/BossLotteryAction.h"

class Boss;

/// <summary>
/// ランチャーを撃つ
/// </summary>
class BossActionShotLauncher :
	public BaseAction<Boss> {
public:

	struct Parameter : public IJsonConverter {
		float bulletSpeed = 80.0f;
		float stiffenTime = 1.0f;

		Parameter() { SetName("BossActionShotLuncher"); }
		
		json ToJson(const std::string& id) const override {
			return JsonBuilder(id)
				.Add("bulletSpeed", bulletSpeed)
				.Add("stiffenTime", stiffenTime)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			fromJson(jsonData, "bulletSpeed", bulletSpeed);
			fromJson(jsonData, "stiffenTime", stiffenTime);
		}
	};

public:

	BossActionShotLauncher() = default;
	~BossActionShotLauncher() override = default;

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

	bool isFinish_;

	std::unique_ptr<BossLotteryAction> weight_;

};


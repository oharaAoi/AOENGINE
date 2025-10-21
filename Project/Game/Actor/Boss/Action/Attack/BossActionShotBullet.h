#pragma once
#include <memory>
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/Module/Components/AI/ITaskNode.h"

class Boss;

/// <summary>
/// 通常弾を撃つ
/// </summary>
class BossActionShotBullet :
	public ITaskNode<Boss> {
public:

	struct Parameter : public IJsonConverter {
		float recoveryTime = 0.1f;
		float shotInterval = 0.1f;
		float bulletSpeed = 80.0f;
		int kFireCount = 20;

		Parameter() { SetName("bossActionShotBullet"); }
		
		json ToJson(const std::string& id) const override {
			return JsonBuilder(id)
				.Add("recoveryTime", recoveryTime)
				.Add("shotInterval", shotInterval)
				.Add("bulletSpeed", bulletSpeed)
				.Add("kFireCount", kFireCount)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			fromJson(jsonData, "recoveryTime", recoveryTime);
			fromJson(jsonData, "shotInterval", shotInterval);
			fromJson(jsonData, "bulletSpeed", bulletSpeed);
			fromJson(jsonData, "kFireCount", kFireCount);
		}

		void Debug_Gui() override;
	};
public:

	BossActionShotBullet() = default;
	~BossActionShotBullet() override = default;

	std::shared_ptr<IBehaviorNode> Clone() const override {
		return std::make_shared<BossActionShotBullet>(*this);
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

	void Shot();

private:

	Parameter param_;

	bool attackStart_;
	bool isFinishShot_;
	int fireCount_;

};


#pragma once
#include <memory>
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/Module/Components/AI/ITaskNode.h"

class Boss;

class BossActionShotgun :
	public ITaskNode<Boss> {
public:

	struct Parameter : public IJsonConverter {
		float bulletSpeed = 80.0f;
		float bulletSpread = 20.0f;
		int kFireCount = 16;

		Parameter() { SetName("BossActionShotgun"); }
		
		json ToJson(const std::string& id) const override {
			return JsonBuilder(id)
				.Add("bulletSpeed", bulletSpeed)
				.Add("bulletSpread", bulletSpread)
				.Add("kFireCount", kFireCount)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			fromJson(jsonData, "bulletSpeed", bulletSpeed);
			fromJson(jsonData, "bulletSpread", bulletSpread);
			fromJson(jsonData, "kFireCount", kFireCount);
		}

		void Debug_Gui() override;
	};

public:

	BossActionShotgun() = default;
	~BossActionShotgun() override = default;

	std::shared_ptr<IBehaviorNode> Clone() const override {
		return std::make_shared<BossActionShotgun>(*this);
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
	bool isFinishShot_;

};


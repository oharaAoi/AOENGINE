#pragma once
#include "Engine/Module/Components/AI/ITaskNode.h"
#include "Engine/Lib/Json/IJsonConverter.h"

class Boss;

class BossActionVerticalMissile :
	public ITaskNode<Boss> {
public:

	struct Parameter : public IJsonConverter {
		float bulletSpeed = 80.0f;
		float fireRadius = 4.0f;
		int kFireCount = 10;

		Parameter() { SetName("BossActionVerticalMissile"); }

		json ToJson(const std::string& id) const override {
			return JsonBuilder(id)
				.Add("bulletSpeed", bulletSpeed)
				.Add("fireRadius", fireRadius)
				.Add("kFireCount", kFireCount)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			fromJson(jsonData, "bulletSpeed", bulletSpeed);
			fromJson(jsonData, "fireRadius", fireRadius);
			fromJson(jsonData, "kFireCount", kFireCount);
		}

		void Debug_Gui() override;
	};

public:

	BossActionVerticalMissile() = default;
	~BossActionVerticalMissile() override = default;

	std::shared_ptr<IBehaviorNode> Clone() const override {
		return std::make_shared<BossActionVerticalMissile>(*this);
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
	int fireCount_;

	float angle_;

};


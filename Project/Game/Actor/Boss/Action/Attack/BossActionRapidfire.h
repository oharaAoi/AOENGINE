#pragma once
#include <memory>
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/Module/Components/AI/ITaskNode.h"

class Boss;

/// <summary>
/// 連射攻撃
/// </summary>
class BossActionRapidfire :
	public ITaskNode<Boss> {
public:

	struct Parameter : public IJsonConverter {
		float shotInterval = 0.1f;
		float bulletSpeed = 80.0f;
		int kFireCount = 20;
		float coolTime = 10.0f;

		Parameter() { SetName("BossActionRapidfire"); }

		json ToJson(const std::string& id) const override {
			return JsonBuilder(id)
				.Add("shotInterval", shotInterval)
				.Add("bulletSpeed", bulletSpeed)
				.Add("kFireCount", kFireCount)
				.Add("coolTime", coolTime)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			fromJson(jsonData, "shotInterval", shotInterval);
			fromJson(jsonData, "bulletSpeed", bulletSpeed);
			fromJson(jsonData, "kFireCount", kFireCount);
			fromJson(jsonData, "coolTime", coolTime);
		}

		void Debug_Gui() override;
	};

public:

	BossActionRapidfire() = default;
	~BossActionRapidfire() override = default;

	std::shared_ptr<IBehaviorNode> Clone() const override {
		return std::make_shared<BossActionRapidfire>(*this);
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

	Parameter param_;

	bool isFinishShot_;
	int fireCount_;

};


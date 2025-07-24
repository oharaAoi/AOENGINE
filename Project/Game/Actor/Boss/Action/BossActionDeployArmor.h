#pragma once
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/Module/Components/AI/ITaskNode.h"

class Boss;

/// <summary>
/// バリアを展開する
/// </summary>
class BossActionDeployArmor :
	public ITaskNode<Boss> {
public:

	struct Parameter : public IJsonConverter {
		float deployTime = 3.0f;
		float coolTime = 20.0f;

		Parameter() { SetName("bossActionDeployArmor"); }
		
		json ToJson(const std::string& id) const override {
			return JsonBuilder(id)
				.Add("deployTime", deployTime)
				.Add("coolTime", coolTime)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			fromJson(jsonData, "deployTime", deployTime);
			fromJson(jsonData, "coolTime", coolTime);
		}
	};
public:

	BossActionDeployArmor();
	~BossActionDeployArmor() override = default;

	std::shared_ptr<IBehaviorNode> Clone() const override {
		return std::make_shared<BossActionDeployArmor>(*this);
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

	Parameter param_;
	bool isDeploy_ = false;

};


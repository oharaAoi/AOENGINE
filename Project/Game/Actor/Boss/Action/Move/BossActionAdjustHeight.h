#pragma once
#include "Engine/Module/Components/AI/ITaskNode.h"

class Boss;

/// <summary>
/// プレイヤーと高さをあわせる
/// </summary>
class BossActionAdjustHeight :
	public ITaskNode<Boss> {
public:

	struct Parameter : public IJsonConverter {
		float smoothTime = 0.1f;	// 追従の速度
		float maxSpeed = 10 ^ 8;	// 追従の最大速度

		Parameter() { SetName("BossActionAdjustHeight"); }

		json ToJson(const std::string& id) const override {
			return JsonBuilder(id)
				.Add("smoothTime", smoothTime)
				.Add("maxSpeed", maxSpeed)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			fromJson(jsonData, "smoothTime", smoothTime);
			fromJson(jsonData, "maxSpeed", maxSpeed);
		}
	};

public:

	BossActionAdjustHeight() = default;
	~BossActionAdjustHeight() override = default;

	std::shared_ptr<IBehaviorNode> Clone() const override {
		return std::make_shared<BossActionAdjustHeight>(*this);
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
	float speed_;

	float distance_;

};


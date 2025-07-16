#pragma once
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/Module/Components/AI/ITaskNode.h"

class Boss;

/// <summary>
/// Bossの近づくアクション
/// </summary>
class BossActionApproach :
	public ITaskNode<Boss> {
public:

	struct Parameter : public IJsonConverter {
		float moveSpeed = 20.f;
		float moveTime = 2.0f;
		float deceleration = 2.f;
		float maxSpinDistance = 0.8f;
		float quitApproachLength = 5.f;
		float decayRate = 5.f;

		Parameter() { SetName("bossActionApproach"); }

		json ToJson(const std::string& id) const override {
			return JsonBuilder(id)
				.Add("moveSpeed", moveSpeed)
				.Add("moveTime", moveTime)
				.Add("deceleration", deceleration)
				.Add("maxSpinDistance", maxSpinDistance)
				.Add("quitApproachLength", quitApproachLength)
				.Add("decayRate", decayRate)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			fromJson(jsonData, "moveSpeed", moveSpeed);
			fromJson(jsonData, "moveTime", moveTime);
			fromJson(jsonData, "deceleration", deceleration);
			fromJson(jsonData, "maxSpinDistance", maxSpinDistance);
			fromJson(jsonData, "quitApproachLength", quitApproachLength);
			fromJson(jsonData, "decayRate", decayRate);
		}
	};

public:

	BossActionApproach() = default;
	~BossActionApproach() = default;

	std::shared_ptr<IBehaviorNode> Clone() const override {
		return std::make_shared<BossActionApproach>(*this);
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

	void SpinApproach();

	void Approach();

private :

	Parameter param_;
	Parameter initParam_;

	bool isShot_;

	// 近づく挙動に必要な変数
	Vector3 toPlayer_;
	float distance_;
	Vector3 direToPlayer_;

	Vector3 lateral_; // player方向に対する垂直なベクトル

	float spinAmount_;	// 旋回量
	Vector3 offsetDire_;

	bool stopping_;

};
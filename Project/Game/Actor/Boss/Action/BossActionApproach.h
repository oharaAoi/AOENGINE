#pragma once
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Game/Actor/Base/BaseAction.h"

class Boss;

class BossActionApproach :
	public BaseAction<Boss> {
public:

	struct Parameter : public IJsonConverter {
		float moveSpeed = 20.f;
		float deceleration = 2.f;
		float maxSpinDistance = 0.8f;
		float quitApproachLength = 5.f;

		json ToJson(const std::string& id) const override {
			return JsonBuilder(id)
				.Add("moveSpeed", moveSpeed)
				.Add("deceleration", deceleration)
				.Add("maxSpinDistance", maxSpinDistance)
				.Add("quitApproachLength", quitApproachLength)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			fromJson(jsonData, "moveSpeed", moveSpeed);
			fromJson(jsonData, "deceleration", deceleration);
			fromJson(jsonData, "maxSpinDistance", maxSpinDistance);
			fromJson(jsonData, "quitApproachLength", quitApproachLength);
		}
	};

public:

	BossActionApproach() = default;
	~BossActionApproach() = default;

	void Build() override;
	void OnStart() override;
	void OnUpdate() override;
	void OnEnd() override;

	void CheckNextAction() override;
	bool IsInput() override;

#ifdef _DEBUG
	void Debug_Gui() override;
#endif // _DEBUG

private:

	void Approach();

private :

	float actionTimer_;

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

};
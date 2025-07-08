#pragma once
#include <memory>
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/Module/Components/AI/ITaskNode.h"
#include "Game/Actor/Boss/BossAI.h"
#include "Game/Actor/Boss/Action/Attack/BossLotteryAction.h"

class Boss;

class BossActionShotMissile :
	public ITaskNode<Boss> {
public:

	struct Parameter : public IJsonConverter {
		AttackActionWeight actionWeight_;

		Parameter() { SetName("bossActionShotMissile"); }

		json ToJson(const std::string& id) const override {
			return JsonBuilder(id)
				.Add("weight", actionWeight_.weight)
				.Add("weightInc", actionWeight_.weightInc)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			fromJson(jsonData, "weight", actionWeight_.weight);
			fromJson(jsonData, "weightInc", actionWeight_.weightInc);
		}
	};

public:

	BossActionShotMissile() = default;
	~BossActionShotMissile() override = default;

	BehaviorStatus Execute() override;

	void Debug_Gui() override;

	bool IsFinish() override;
	bool CanExecute() override;

	void Init() override;
	void Update() override;
	void End() override;

private:

	void Shot();

private :

	// 弾を打ち終わったかのフラグ
	bool isFinishShot_;
	float shotInterval_;

	float bulletSpeed_;

	const uint32_t kFireCount_ = 4;
	uint32_t fireCount_;

	std::unique_ptr<BossLotteryAction> weight_;

};
#pragma once
#include <memory>
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/Module/Components/AI/ITaskNode.h"

class Boss;

class BossActionShotMissile :
	public ITaskNode<Boss> {
public:

	struct Parameter : public IJsonConverter {
		float shotInterval = 0.1f;
		float bulletSpeed = 100.0f;

		Parameter() { SetName("bossActionShotMissile"); }

		json ToJson(const std::string& id) const override {
			return JsonBuilder(id)
				.Add("shotInterval", shotInterval)
				.Add("bulletSpeed", bulletSpeed)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			fromJson(jsonData, "shotInterval", shotInterval);
			fromJson(jsonData, "bulletSpeed", bulletSpeed);
		}

		void Debug_Gui() override;
	};

public:

	BossActionShotMissile() = default;
	~BossActionShotMissile() override = default;

	std::shared_ptr<IBehaviorNode> Clone() const override {
		return std::make_shared<BossActionShotMissile>(*this);
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

private :
	Parameter param_;

	// 弾を打ち終わったかのフラグ
	bool isFinishShot_;

	const uint32_t kFireCount_ = 4;
	uint32_t fireCount_;

};
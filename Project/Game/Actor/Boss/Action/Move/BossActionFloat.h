#pragma once
#include "Engine/Module/Components/AI/ITaskNode.h"
#include "Engine/Lib/Json/IJsonConverter.h"

class Boss;


class BossActionFloat :
	public ITaskNode<Boss>{

public:

	struct Parameter : public IJsonConverter {
		float moveTime = 0.5f;
		float moveSpeed = 10.0f;

		Parameter() { SetName("BossActionFloat"); }

		json ToJson(const std::string& id) const override {
			return JsonBuilder(id)
				.Add("moveTime", moveTime)
				.Add("moveSpeed", moveSpeed)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			fromJson(jsonData, "moveTime", moveTime);
			fromJson(jsonData, "moveSpeed", moveSpeed);
		}

		void Debug_Gui() override;
	};

public:

	BossActionFloat() = default;
	~BossActionFloat() override = default;

	std::shared_ptr<IBehaviorNode> Clone() const override {
		return std::make_shared<BossActionFloat>(*this);
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
};


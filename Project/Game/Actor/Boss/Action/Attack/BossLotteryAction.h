#pragma once
#include <string>
#include "Engine/Lib/Json/IJsonConverter.h"

struct AttackActionWeight {
	float weight = 10.0f;
	float weightInc = 10.0f;
};

class BossLotteryAction {
public:

	struct Parameter : public IJsonConverter {
		AttackActionWeight actionWeight_;

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

	BossLotteryAction() = default;
	~BossLotteryAction() = default;

	void Init(const std::string& actionName);

	void Debug_Gui();

	const std::string& GetParameterName() { return weightParameter_.GetName(); }

	void Reset();

	void Increase();

	float GetWeight() const { return weightParameter_.actionWeight_.weight; }

private :

	Parameter weightParameter_;
	Parameter initWeightParameter_;

};


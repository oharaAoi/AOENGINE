#pragma once
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Game/Actor/Base/BaseAction.h"

class Boss;

/// <summary>
/// バリアを展開する
/// </summary>
class BossActionDeployArmor :
	public BaseAction<Boss> {
public:

	struct Parameter : public IJsonConverter {
		float deployTime = 3.0f;

		Parameter() { SetName("bossActionDeployArmor"); }
		
		json ToJson(const std::string& id) const override {
			return JsonBuilder(id)
				.Add("deployTime", deployTime)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			fromJson(jsonData, "deployTime", deployTime);
		}
	};
public:

	BossActionDeployArmor() = default;
	~BossActionDeployArmor() override = default;

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

	Parameter param_;

};


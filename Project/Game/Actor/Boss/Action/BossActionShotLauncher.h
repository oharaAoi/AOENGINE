#pragma once
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Game/Actor/Base/BaseAction.h"

class Boss;

/// <summary>
/// ランチャーを撃つ
/// </summary>
class BossActionShotLauncher :
	public BaseAction<Boss> {
public:

	struct Parameter : public IJsonConverter {
		float bulletSpeed = 80.0f;
		float stiffenTime = 1.0f;
		
		json ToJson(const std::string& id) const override {
			return JsonBuilder(id)
				.Add("bulletSpeed", bulletSpeed)
				.Add("stiffenTime", stiffenTime)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			fromJson(jsonData, "bulletSpeed", bulletSpeed);
			fromJson(jsonData, "stiffenTime", stiffenTime);
		}
	};
public:

	BossActionShotLauncher() = default;
	~BossActionShotLauncher() override = default;

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

	void Shot();

private:

	Parameter param_;

	bool isFinish_;

};


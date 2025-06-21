#pragma once
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Game/Actor/Base/BaseAction.h"

class Boss;

class BossActionShotgun :
	public BaseAction<Boss> {
public:

	struct Parameter : public IJsonConverter {
		float bulletSpeed = 80.0f;
		float bulletSpread = 20.0f;
		int kFireCount = 16;

		Parameter() { SetName("BossActionShotgun"); }
		
		json ToJson(const std::string& id) const override {
			return JsonBuilder(id)
				.Add("bulletSpeed", bulletSpeed)
				.Add("bulletSpread", bulletSpread)
				.Add("kFireCount", kFireCount)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			fromJson(jsonData, "bulletSpeed", bulletSpeed);
			fromJson(jsonData, "bulletSpread", bulletSpread);
			fromJson(jsonData, "kFireCount", kFireCount);
		}
	};

public:

	BossActionShotgun() = default;
	~BossActionShotgun() override = default;

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
	bool isFinishShot_;
};


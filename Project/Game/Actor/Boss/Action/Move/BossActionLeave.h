#pragma once
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Game/Actor/Base/BaseAction.h"

class Boss;

/// <summary>
/// Bossの離れるアクション
/// </summary>
class BossActionLeave :
	public BaseAction<Boss> {
public:	// 構造体

	struct Parameter : public IJsonConverter {
		float moveSpeed = 40.f;
		float moveTime = 1.5f;

		Parameter() { SetName("bossActionLeave"); }

		json ToJson(const std::string& id) const override {
			return JsonBuilder(id)
				.Add("moveSpeed", moveSpeed)
				.Add("moveTime", moveTime)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			fromJson(jsonData, "moveSpeed", moveSpeed);
			fromJson(jsonData, "moveTime", moveTime);
		}
	};

public:

	BossActionLeave() = default;
	~BossActionLeave() = default;

	void Build() override;
	void OnStart() override;
	void OnUpdate() override;
	void OnEnd() override;

	void CheckNextAction() override;
	bool IsInput() override;

	void Debug_Gui() override;

private:

	void Leave();

private:

	Parameter param_;

	Vector3 velocity_;
	Vector3 accel_;

};


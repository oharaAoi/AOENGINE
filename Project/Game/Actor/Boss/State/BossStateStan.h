#pragma once
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Game/State/ICharacterState.h"
#include "Engine/Lib/Math/MyMath.h"

class Boss;

/// <summary>
/// Bossのスタン状態
/// </summary>
class BossStateStan :
	public ICharacterState<Boss> {
public:	// メンバ構造体

	struct Parameter : public IJsonConverter {
		float stanTime = 3.0f;
		float fallTime = 1.0f;

		Parameter() { SetName("BossStanParameter"); }

		json ToJson(const std::string& id) const override {
			return JsonBuilder(id)
				.Add("stanTime", stanTime)
				.Add("fallTime", fallTime)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			fromJson(jsonData, "stanTime", stanTime);
			fromJson(jsonData, "fallTime", fallTime);
		}
	};

public:

	BossStateStan() = default;
	~BossStateStan() = default;

	void OnStart() override;
	void OnUpdate() override;
	void OnExit() override;

	void Debug_Gui() override;

public:

	/// <summary>
	/// 重力の適用
	/// </summary>
	void ApplyGravity();

private:

	Parameter param_;

	float stateTime_;

	Vector3 velocity_;
	Vector3 acceleration_ = { 0.0f,kGravity, 0.0f };

};


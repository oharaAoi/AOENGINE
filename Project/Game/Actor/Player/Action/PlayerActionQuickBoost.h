#pragma once
#include "Engine/System/Input/Input.h"
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/Module/Components/WorldTransform.h"
#include "Engine/Module/Components/Effect/BaseParticles.h"
#include "Game/Actor/Base/BaseAction.h"

// 前方宣言
class Player;

/// <summary>
/// クイックブーストを行うクラス
/// </summary>
class PlayerActionQuickBoost :
	public BaseAction<Player> {
public:

	struct Parameter : public IJsonConverter {
		float boostForce = 10.0f;		// boostの強さ
		float decelerationRaito = 0.8f;	// 減速の割合

		float boostEnergy = 20.0f;		// 消費エネルギー

		float cameraShakeTime = 0.2f;
		float cameraShakeStrength = 0.1f;

		Parameter() { SetName("ActionQuickBoost"); }

		json ToJson(const std::string& id) const override {
			return JsonBuilder(id)
				.Add("boostForce", boostForce)
				.Add("decelerationRaito", decelerationRaito)
				.Add("boostEnergy", boostEnergy)
				.Add("cameraShakeTime", cameraShakeTime)
				.Add("cameraShakeStrength", cameraShakeStrength)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			fromJson(jsonData, "boostForce", boostForce);
			fromJson(jsonData, "decelerationRaito", decelerationRaito);
			fromJson(jsonData, "boostEnergy", boostEnergy);
			fromJson(jsonData, "cameraShakeTime", cameraShakeTime);
			fromJson(jsonData, "cameraShakeStrength", cameraShakeStrength);
		}
	};

public:

	PlayerActionQuickBoost() = default;
	~PlayerActionQuickBoost() override {};

	void Build() override;
	void OnStart() override;
	void OnUpdate() override;
	void OnEnd() override;

	void CheckNextAction() override;
	bool IsInput() override;

	void Debug_Gui() override;

private:

	/// <summary>
	/// ブーストを行う
	/// </summary>
	void Boost();

private:

	// 他クラス ------------------------------------------------
	Input* pInput_;
	WorldTransform* pOwnerTransform_ = nullptr;

	// State/Parameter ------------------------------------------------
	Vector3 velocity_;
	Vector3 acceleration_ = { 0.0f, 0.0f, 0.0f };
	Vector3 direction_;

	const float kDeadZone_ = 0.1f;
	Vector2 stick_;

	Parameter param_;
	Parameter initParam_;

	BaseParticles* boostParticle_;

};


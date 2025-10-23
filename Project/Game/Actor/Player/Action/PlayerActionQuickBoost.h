#pragma once
#include "Engine/System/Input/Input.h"
#include "Engine/Lib/Math/Curve.h"
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/Module/Components/WorldTransform.h"
#include "Engine/Module/Components/Physics/Rigidbody.h"
#include "Engine/Module/Components/Effect/BaseParticles.h"
#include "Engine/Module/PostEffect/RadialBlur.h"
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

		float decelerationTime = 1.0f;		// 減速までの時間
		Curve decelerationCurve;	// 減速の際のカーブ

		Parameter() { SetName("ActionQuickBoost"); }

		json ToJson(const std::string& id) const override {
			json curveJson = decelerationCurve.ToJson();
			return JsonBuilder(id)
				.Add("boostForce", boostForce)
				.Add("decelerationRaito", decelerationRaito)
				.Add("boostEnergy", boostEnergy)
				.Add("cameraShakeTime", cameraShakeTime)
				.Add("cameraShakeStrength", cameraShakeStrength)
				.Add("decelerationTime", decelerationTime)
				.Add("decelerationCurve", curveJson)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			fromJson(jsonData, "boostForce", boostForce);
			fromJson(jsonData, "decelerationRaito", decelerationRaito);
			fromJson(jsonData, "boostEnergy", boostEnergy);
			fromJson(jsonData, "cameraShakeTime", cameraShakeTime);
			fromJson(jsonData, "cameraShakeStrength", cameraShakeStrength);
			fromJson(jsonData, "decelerationTime", decelerationTime);
			decelerationCurve.FromJson(jsonData, "decelerationCurve");
		}

		void Debug_Gui() override;
	};

public:

	PlayerActionQuickBoost() = default;
	~PlayerActionQuickBoost() override {};

public:

	// ビルド処理
	void Build() override;
	// 初期化
	void OnStart() override;
	// 更新
	void OnUpdate() override;
	// 終了処理
	void OnEnd() override;
	// 次のアクションの判定
	void CheckNextAction() override;
	// actionの入力判定
	bool IsInput() override;
	// 編集処理
	void Debug_Gui() override;

private:

	/// <summary>
	/// ブーストを行う
	/// </summary>
	void Boost();

	/// <summary>
	/// 減速を行う
	/// </summary>
	void Deceleration();

private:

	// 他クラス ------------------------------------------------
	Input* pInput_;
	WorldTransform* pOwnerTransform_ = nullptr;
	std::shared_ptr<RadialBlur> pRadialBlur_;

	// State/Parameter ------------------------------------------------
	Vector3 acceleration_ = { 0.0f, 0.0f, 0.0f };
	Vector3 direction_;

	const float kDeadZone_ = 0.1f;
	Vector2 stick_;

	Parameter param_;
	Parameter initParam_;

	BaseParticles* boostParticle_;

	Rigidbody* pRigidBody_;

};


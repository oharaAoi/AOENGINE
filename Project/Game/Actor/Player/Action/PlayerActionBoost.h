#pragma once
#include <functional>
// Engine
#include "Engine/System/Input/Input.h"
#include "Engine/Module/Components/WorldTransform.h"
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/Module/PostEffect/RadialBlur.h"
#include "Engine/Module/Components/Physics/Rigidbody.h"
// Game
#include "Game/Actor/Base/BaseAction.h"
#include "Game/Camera/Animation/ICameraAnimation.h"

// 前方宣言
class Player;

/// <summary>
/// Boostを行うクラス
/// </summary>
class PlayerActionBoost :
	public BaseAction<Player> {
public:

	struct Parameter : public IJsonConverter {
		float chargeTime;		// 溜める時間
	
		float boostForce;		// ブーストの力
		float stopForce;		// 止める力

		float bluerStrength;	// ブラーの強さ
		float bluerStartTime;	// ブラーがかかるまでの時間
		float bluerStopTime;	// ブラーが止まるまでの時間

		float consumeEnergy;	// 消費エネルギー

		Parameter() { 
			SetGroupName("PlayerAction");
			SetName("ActionBoost"); 
		}

		json ToJson(const std::string& id) const override {
			return JsonBuilder(id)
				.Add("chargeTime", chargeTime)
				.Add("boostForce", boostForce)
				.Add("stopForce", stopForce)
				.Add("bluerStrength", bluerStrength)
				.Add("bluerStartTime", bluerStartTime)
				.Add("bluerStopTime", bluerStopTime)
				.Add("consumeEnergy", consumeEnergy)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			fromJson(jsonData, "chargeTime", chargeTime);
			fromJson(jsonData, "boostForce", boostForce);
			fromJson(jsonData, "stopForce", stopForce);
			fromJson(jsonData, "bluerStrength", bluerStrength);
			fromJson(jsonData, "bluerStartTime", bluerStartTime);
			fromJson(jsonData, "bluerStopTime", bluerStopTime);
			fromJson(jsonData, "consumeEnergy", consumeEnergy);
		}

		void Debug_Gui() override;
	};

public:

	PlayerActionBoost() = default;
	~PlayerActionBoost() override;

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
	/// ブーストの溜めを行う
	/// </summary>
	void BoostCharge();

	/// <summary>
	/// ブーストの中止
	/// </summary>
	void BoostStop();

	/// <summary>
	/// 中止確認
	/// </summary>
	/// <returns></returns>
	bool CheckStop();

private:

	// 他クラス ------------------------------------------------
	Input* pInput_;
	Rigidbody* pRigidbody_ = nullptr;
	WorldTransform* pOwnerTransform_ = nullptr;
	ICameraAnimation* pCameraAnimation_ = nullptr;

	// State/Parameter ------------------------------------------------
	Parameter param_;
	Parameter initialPram_;
	
	Vector3 acceleration_ = { 0.0f, 0.0f, 0.0f };
	Vector3 direction_;

	// boost
	std::function<void()> mainAction_;
	bool isStop_ = false;
	
	// time
	float timer_;

	// input
	const float kDeadZone_ = 0.1f;
	Vector2 stick_;

	std::shared_ptr<RadialBlur> blur_;

};


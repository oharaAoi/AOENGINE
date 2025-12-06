#pragma once
#include "Game/Actor/Base/BaseAction.h"
#include "Engine/Lib/Math/MyMath.h"
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/Module/Components/Physics/Rigidbody.h"
#include "Engine/Module/Components/WorldTransform.h"
#include "Engine/Module/Components/Effect/BaseParticles.h"

// 前方宣言
class Player;

/// <summary>
/// Playerのジャンプ行動
/// </summary>
class PlayerActionJump :
	public BaseAction<Player> {
public:

	struct Parameter : public AOENGINE::IJsonConverter {
		float chargeTime = 0.5f;
		float finishChargeTime = 1.0f;
		float jumpForce = 6.0f;
		float risingForce = 10.0f;
		float maxAcceleration = 20.0f;
		float accelDecayRate = 2.0f;
		float velocityDecayRate = 2.0f;

		float jumpEnergy = 5.0f;		// 消費エネルギー

		float cameraShakeTime = 0.2f;
		float cameraShakeStrength = 0.1f;

		float animationBlendTime = 0.5f;

		Parameter() { SetName("ActionJump"); }

		json ToJson(const std::string& id) const override {
			return AOENGINE::JsonBuilder(id)
				.Add("chargeTime", chargeTime)
				.Add("finishChargeTime", finishChargeTime)
				.Add("jumpForce", jumpForce)
				.Add("risingForce", risingForce)
				.Add("maxAcceleration", maxAcceleration)
				.Add("accelDecayRate", accelDecayRate)
				.Add("velocityDecayRate", velocityDecayRate)
				.Add("jumpEnergy", jumpEnergy)
				.Add("cameraShakeTime", cameraShakeTime)
				.Add("cameraShakeStrength", cameraShakeStrength)
				.Add("animationBlendTime", animationBlendTime)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			fromJson(jsonData, "chargeTime", chargeTime);
			fromJson(jsonData, "finishChargeTime", finishChargeTime);
			fromJson(jsonData, "jumpForce", jumpForce);
			fromJson(jsonData, "risingForce", risingForce);
			fromJson(jsonData, "maxAcceleration", maxAcceleration);
			fromJson(jsonData, "accelDecayRate", accelDecayRate);
			fromJson(jsonData, "velocityDecayRate", velocityDecayRate);
			fromJson(jsonData, "jumpEnergy", jumpEnergy);
			fromJson(jsonData, "cameraShakeTime", cameraShakeTime);
			fromJson(jsonData, "cameraShakeStrength", cameraShakeStrength);
			fromJson(jsonData, "animationBlendTime", animationBlendTime);
		}

		void Debug_Gui() override;
	};

public:

	PlayerActionJump() = default;
	~PlayerActionJump() override {};

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

private:	// action

	/// <summary>
	/// ちゃんとジャンプする前のジャンプ
	/// </summary>
	void SmallJump();

	/// <summary>
	/// main action
	/// </summary>
	void Jump();

	/// <summary>
	/// 上昇する
	/// </summary>
	void Rising();

	/// <summary>
	/// チャージする
	/// </summary>
	void Charge();

private:	// variable

	Math::Vector3 acceleration_ = { 0.0f,kGravity, 0.0f };

	Parameter param_;

	WorldTransform* pOwnerTransform_ = nullptr;

	std::function<void()> mainAction_;
	
	// 小ジャンプに関する変数
	float smallJumpTime_ = 0.2f;

	// particleに関する変数
	AOENGINE::BaseParticles* jetBurnLeft_;
	AOENGINE::BaseParticles* jetBurnRight_;

	Math::Matrix4x4 feetMatrixLeft_;
	Math::Matrix4x4 feetMatrixRight_;

	AOENGINE::Rigidbody* pRigidbody_;

};


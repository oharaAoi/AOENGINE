#pragma once
#include "Game/Actor/Base/BaseAction.h"
#include "Engine/Lib/Math/MyMath.h"
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/Module/Components/WorldTransform.h"

// 前方宣言
class Player;

/// <summary>
/// Playerのジャンプ行動
/// </summary>
class PlayerActionJump :
	public BaseAction<Player> {
public:

	struct Parameter : public IJsonConverter {
		float jumpForce = 6.0f;
		float risingForce = 10.0f;
		float maxAcceleration = 20.0f;

		float jumpEnergy = 5.0f;		// 消費エネルギー

		json ToJson(const std::string& id) const override {
			return JsonBuilder(id)
				.Add("jumpForce", jumpForce)
				.Add("risingForce", risingForce)
				.Add("maxAcceleration", maxAcceleration)
				.Add("jumpEnergy", jumpEnergy)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			fromJson(jsonData, "jumpForce", jumpForce);
			fromJson(jsonData, "risingForce", risingForce);
			fromJson(jsonData, "maxAcceleration", maxAcceleration);
			fromJson(jsonData, "jumpEnergy", jumpEnergy);
		}
	};

public:

	PlayerActionJump() = default;
	~PlayerActionJump() override {};

	void Build() override;
	void OnStart() override;
	void OnUpdate() override;
	void OnEnd() override;

	void CheckNextAction() override;
	bool IsInput() override;

#ifdef _DEBUG
	void Debug_Gui() override;
#endif // _DEBUG

private:	// action

	/// <summary>
	/// main action
	/// </summary>
	void Jump();

	/// <summary>
	/// 上昇する
	/// </summary>
	void Rising();

	/// <summary>
	/// 重力の適用
	/// </summary>
	void ApplyGravity();

private:	// variable

	Vector3 velocity_;
	Vector3 acceleration_ = { 0.0f,kGravity, 0.0f };

	Parameter param_;
	bool isFall_;

	bool isRising_;
	bool isPreRising_;

	WorldTransform* pOwnerTransform_ = nullptr;

};


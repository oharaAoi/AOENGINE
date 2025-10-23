#pragma once
#include <functional>
#include "Engine/System/Input/Input.h"
#include "Engine/Module/Components/WorldTransform.h"
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Game/Actor/Base/BaseAction.h"
#include "Engine/Module/PostEffect/RadialBlur.h"

// 前方宣言
class Player;

/// <summary>
/// Boostを行うクラス
/// </summary>
class PlayerActionBoost :
	public BaseAction<Player> {
public:

	struct Parameter : public IJsonConverter {
		float chargeTime;
		float chargeForce;

		float boostForce;
		float stopForce;

		Parameter() { SetName("actionBoost"); }

		json ToJson(const std::string& id) const override {
			return JsonBuilder(id)
				.Add("chargeTime", chargeTime)
				.Add("chargeForce", chargeForce)
				.Add("boostForce", boostForce)
				.Add("stopForce", stopForce)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			fromJson(jsonData, "chargeTime", chargeTime);
			fromJson(jsonData, "chargeForce", chargeForce);
			fromJson(jsonData, "boostForce", boostForce);
			fromJson(jsonData, "stopForce", stopForce);
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
	WorldTransform* pOwnerTransform_ = nullptr;

	// State/Parameter ------------------------------------------------
	Parameter param_;
	Parameter initialPram_;
	
	Vector3 velocity_;
	Vector3 acceleration_ = { 0.0f, 0.0f, 0.0f };
	Vector3 direction_;

	// boost
	std::function<void()> mainAction_;
	bool finishBoost_;

	// time
	float timer_;

	// input
	const float kDeadZone_ = 0.1f;
	Vector2 stick_;

	std::shared_ptr<RadialBlur> blur_;

};


#pragma once
#include "Engine/Module/Components/Physics/Rigidbody.h"
#include "Game/Actor/Base/BaseAction.h"
#include <Lib/Json/IJsonConverter.h>

// 前方宣言
class Player;

/// <summary>
/// 移動アクション
/// </summary>
class PlayerActionMove :
	public BaseAction<Player> {
public:

	struct Parameter : public IJsonConverter {
		float speed;
		float boostSpeed;
		float moveT = 0.5f;
		float rotateT = 0.5f;
		float decayRate = 2.0f;

		Parameter() { SetName("ActionMove"); }

		json ToJson(const std::string& id) const override {
			return JsonBuilder(id)
				.Add("speed", speed)
				.Add("boostSpeed", boostSpeed)
				.Add("moveT", moveT)
				.Add("rotateT", rotateT)
				.Add("decayRate", decayRate)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			fromJson(jsonData, "speed", speed);
			fromJson(jsonData, "boostSpeed", boostSpeed);
			fromJson(jsonData, "moveT", moveT);
			fromJson(jsonData, "rotateT", rotateT);
			fromJson(jsonData, "decayRate", decayRate);
		}

		void Debug_Gui() override;
	};

public:

	PlayerActionMove() = default;
	~PlayerActionMove() override {};

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
	/// 動く処理
	/// </summary>
	void Move();

	/// <summary>
	/// プレイヤーの反転処理
	/// </summary>
	/// <param name="currentVelocity"></param>
	/// <returns></returns>
	bool IsDirectionReversed(const Vector3& currentVelocity);

private:

	// Parameter -------------------------------

	Rigidbody* pRigidbody_ = nullptr;

	const float kDeadZone_ = 0.1f;
	Vector2 inputStick_;

	Vector3 accel_;
	Vector3 velocity_;
	Vector3 preVelocity_;

	Parameter param_;

	bool isTurnAround_;

	bool preBoost_;
};


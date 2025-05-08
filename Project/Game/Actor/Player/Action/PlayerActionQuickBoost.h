#pragma once
#include "Engine/System/Input/Input.h"
#include "Engine/Module/Components/WorldTransform.h"
#include "Game/Actor/Base/BaseAction.h"

// 前方宣言
class Player;

/// <summary>
/// クイックブーストを行うクラス
/// </summary>
class PlayerActionQuickBoost :
	public BaseAction<Player> {
public:

	struct Parameter {
		float boostForce = 10.0f;
		float decelerationRaito = 0.8f;
	};

public:

	PlayerActionQuickBoost() = default;
	~PlayerActionQuickBoost() = default;

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
	Parameter firstParam_;

};


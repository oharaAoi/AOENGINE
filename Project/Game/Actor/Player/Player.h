#pragma once
#include "Engine/Module/Components/GameObject/BaseEntity.h"
#include "Engine/Lib/Math/Vector3.h"
#include "Game/Actor/Player/PlayerParameter.h"

/// <summary>
/// プレイヤークラス
/// </summary>
class Player : public AOENGINE::BaseEntity {
public:

	// ジャンプの状態
	enum class JumpState {
		Grounded,	// 接地
		Rising,		// 上昇中
		Hanging,	// 頂点で滞空中
		Falling,	// 落下中
	};

public:

	Player() = default;
	~Player() override = default;
	Player(const Player&) = delete;
	Player& operator=(const Player&) = delete;

	// 初期化、更新
	void Init(AOENGINE::BaseGameObject* body);
	void Update();

	// デバッグ描画
	void Debug_Gui();

private:

	void InputMove();
	void UpdateLaunch(float deltaTime);
	void UpdateJump(float deltaTime);
	void ResolveGround();
	void ApplyToBody();

	float ReadHorizontal() const;
	bool  IsJumpTriggered() const;
	bool  IsLaunchTriggered() const;

private:

	// 調整項目
	PlayerParameter parameter_;

	// 位置、速度
	Math::Vector3 position_{};
	Math::Vector3 velocity_{};

	// ジャンプ状態
	JumpState jumpState_ = JumpState::Grounded;
	float hangTimer_ = 0.0f;

	bool  isLaunching_ = false;
	float launchTimer_ = 0.0f;

	float facing_ = 1.0f;

public: // accessor

	bool IsGrounded() const { return jumpState_ == JumpState::Grounded; }
	bool IsLaunching() const { return isLaunching_; }

	const Math::Vector3& GetSimPosition() const { return position_; }
	const Math::Vector3& GetVelocity() const { return velocity_; }
	float GetFacing() const { return facing_; }
};

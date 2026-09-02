#pragma once
#include "Engine/Module/Components/GameObject/BaseEntity.h"
#include "Engine/Lib/Math/Vector3.h"
#include "Game/Actor/Player/PlayerParameter.h"
#include "Game/Actor/Player/Component/PlayerInput.h"
#include "Game/Actor/Player/Component/PlayerJump.h"

/// <summary>
/// プレイヤークラス
/// </summary>
class Player : public AOENGINE::BaseEntity {
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

	void UpdateMove();
	void ResolveGround();
	void ApplyToBody();

private:

	// 調整項目
	PlayerParameter parameter_;

	// コンポーネント
	PlayerInput input_;
	PlayerJump  jump_;

	// 位置、速度
	Math::Vector3 position_{};
	Math::Vector3 velocity_{};

	// 最後に向いた左右方向
	float facing_ = 1.0f;

public: // accessor

	bool IsGrounded() const { return jump_.IsGrounded(); }

	const Math::Vector3& GetSimPosition() const { return position_; }
	const Math::Vector3& GetVelocity() const { return velocity_; }
	float GetFacing() const { return facing_; }
};

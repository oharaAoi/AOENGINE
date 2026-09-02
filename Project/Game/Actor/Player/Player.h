#pragma once
#include "Engine/Module/Components/GameObject/BaseEntity.h"
#include "Engine/Lib/Math/Vector3.h"
#include "Game/Actor/Player/PlayerParameter.h"
#include "Game/Actor/Player/Component/PlayerInput.h"
#include "Game/Actor/Player/Component/PlayerJump.h"
#include "Game/Actor/Player/Component/BlockGroupConnectState.h"

namespace AOENGINE {
class Rigidbody;
}

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

	/// <summary>本体のBaseGameObjectが持つRigidbodyを取得する。</summary>
	AOENGINE::Rigidbody* GetRigidbody() const;

	void UpdateMove(AOENGINE::Rigidbody* rigidbody);
	void ResolveGround();
	/// <summary>Colliderの押し戻し方向から足場に乗っているかを判定する。</summary>
	bool ResolvePushback();

private:

	// 調整項目
	PlayerParameter parameter_;

	// コンポーネント
	PlayerInput input_;
	PlayerJump  jump_;
	BlockGroupConnectState blockGroupConnectState_;

	// 最後に向いた左右方向
	float facing_ = 1.0f;

	// 自分のCollider category名
	static inline const std::string kColliderTag = "Player";
	// 押し戻しを接地・天井とみなす閾値
	static constexpr float kPushbackThreshold = 0.0001f;

public: // accessor

	bool IsGrounded() const { return jump_.IsGrounded(); }

	// 落下中（着地しうる状態）かどうか
	bool IsFalling() const { return jump_.GetState() == PlayerJump::State::Falling || jump_.GetState() == PlayerJump::State::Hanging; }

	// 着地したブロックグループを接続対象へ追加する
	bool TryConnectBlockGroup(int groupId) { return blockGroupConnectState_.TryAdd(groupId); }

	Math::Vector3 GetVelocity() const;
	float GetFacing() const { return facing_; }
};

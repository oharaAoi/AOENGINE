#pragma once
#include <vector>
#include "Engine/Module/Components/GameObject/BaseEntity.h"
#include "Engine/Lib/Math/Vector3.h"
#include "Game/Actor/Player/PlayerParameter.h"
#include "Game/Actor/Player/Component/PlayerInput.h"
#include "Game/Actor/Player/Component/PlayerJump.h"
#include "Game/Actor/Player/Component/BlockGroupConnectState.h"
#include "Game/Stage/BlockGroupLauncher.h"

namespace AOENGINE {
	class Rigidbody;
}

class StageBlockField;
class Block;

/// <summary>
/// プレイヤークラス
/// </summary>
class Player : public AOENGINE::BaseEntity{
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

	void UpdateMove(AOENGINE::Rigidbody* rigidbody, float deltaTime);
	/// <summary>接続受付・集合・打ち上げの一連の流れを進める。</summary>
	void UpdateBlockGroupConnect(float deltaTime);
	/// <summary>接続したブロックグループ同士を線で結んで描画する。</summary>
	void DrawBlockGroupConnectLine() const;
	void ResolveGround();
	/// <summary>Colliderの押し戻し方向から足場に乗っているかを判定する。</summary>
	bool ResolvePushback();
	/// <summary>大ジャンプ中はBlockとの当たり判定だけを外す</summary>
	void SetBlockCollisionEnabled(bool enabled);
	/// <summary>自分のColliderと現在重なっているBlockを列挙する</summary>
	std::vector<Block*> GetOverlappingBlocks() const;
	/// <summary>Block判定を再開するが、今まさに埋まっているBlockだけは個別に無効化して猶予を与える</summary>
	void ResumeBlockCollision();
	/// <summary>猶予中のBlockを毎フレーム見直し、離れたものから判定を戻す</summary>
	void UpdateIgnoredBlocks();

private:

	// 調整項目
	PlayerParameter parameter_;

	// コンポーネント
	PlayerInput input_;
	PlayerJump  jump_;
	BlockGroupConnectState blockGroupConnectState_;
	BlockGroupLauncher     blockGroupLauncher_;

	// 集合・打ち上げの対象を引き当てる連結グループ表(非所有)
	StageBlockField* pBlockField_ = nullptr;

	// 最後に向いた左右方向
	float facing_ = 1.0f;

	// ダメージ床で打ち上げられてから着地するまでtrue
	bool damageFloorAirborne_ = false;
	// 今回の大ジャンプで、落下開始時のBlock判定再開をもう済ませたか
	bool blockCollisionResumed_ = false;
	// 直前フレームのジャンプ状態
	PlayerJump::State previousJumpState_ = PlayerJump::State::Grounded;
	// Block判定を無効化のBlock
	std::vector<Block*> ignoredBlocks_;

	// 自分のCollider category名
	static inline const std::string kColliderTag = "Player";
	// 大ジャンプ中に無視するCollider category名
	static inline const std::string kBlockCategoryName = "Block";
	// 押し戻しを接地・天井とみなす閾値
	static constexpr float kPushbackThreshold = 0.0001f;

public: // accessor
	const BlockGroupLauncher* GetBlockGroupLauncher() const{ return &blockGroupLauncher_; }
	BlockGroupLauncher* GetBlockGroupLauncherRef(){ return &blockGroupLauncher_; }

	bool IsGrounded() const{ return jump_.IsGrounded(); }

	// 落下中（着地しうる状態）かどうか
	bool IsFalling() const{ return jump_.GetState() == PlayerJump::State::Falling || jump_.GetState() == PlayerJump::State::Hanging; }

	// 着地したブロックグループを接続対象へ追加する。追加できたグループは色が変わる
	bool TryConnectBlockGroup(int groupId);

	// ダメージ床に当たった時のノックバックを開始する
	void ApplyDamageFloorKnockback(float power);
	// ダメージ床で打ち上げられてから着地するまでの間か
	bool IsDamageFloorAirborne() const { return damageFloorAirborne_; }

	// 集合・打ち上げでグループを引き当てるための連結グループ表を設定する(非所有)
	void SetBlockField(StageBlockField* field);

	Math::Vector3 GetVelocity() const;
	float GetFacing() const{ return facing_; }
};

#pragma once
#include <string>
#include <vector>
#include "Engine/Module/Components/GameObject/BaseEntity.h"
#include "Engine/Lib/Math/Vector3.h"
#include "Engine/Lib/Math/Quaternion.h"
#include "Game/Actor/Player/PlayerParameter.h"
#include "Game/Actor/Player/Component/PlayerInput.h"
#include "Game/Actor/Player/Component/PlayerJump.h"
#include "Game/Actor/Player/Component/BlockGroupConnectState.h"
#include "Game/Stage/BlockGroupLauncherManager.h"

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
	void ResolveGround(float deltaTime);
	/// <summary>Colliderの押し戻し方向から足場に乗っているかを判定する。</summary>
	bool ResolvePushback();
	/// <summary>
	/// 足元の少し下を見て、そこにあるブロックの上面の高さを求める。
	/// </summary>
	/// <param name="checkDown">足元から何ユニット下まで見るか</param>
	/// <param name="outTopY">一番高いブロックの上面</param>
	/// <returns>ブロックが見つかったら true</returns>
	bool TryGetGroundTop(float checkDown, float& outTopY) const;
	/// <summary>接地中は、足元のブロックの上面へ直接置き直す。</summary>
	void SnapToGround();
	/// <summary>大ジャンプ中はBlockとの当たり判定だけを外す</summary>
	void SetBlockCollisionEnabled(bool enabled);
	/// <summary>自分のColliderと現在重なっているBlockを列挙する</summary>
	std::vector<Block*> GetOverlappingBlocks() const;
	/// <summary>Block判定を再開するが、今まさに埋まっているBlockだけは個別に無効化して猶予を与える</summary>
	void ResumeBlockCollision();
	/// <summary>猶予中のBlockを毎フレーム見直し、離れたものから判定を戻す</summary>
	void UpdateIgnoredBlocks();
	/// <summary>無敵時間を進めつつ、その間ちかちか点滅させる</summary>
	void UpdateInvincible(float deltaTime);
	/// <summary>落下の下限。これより下がったらその高さで止める</summary>
	void ClampFallLimit();
	/// <summary>押し戻しで奥行きがずれても、決まったZへ固定し直す</summary>
	void FixZPosition();
	/// <summary>基準スケールに演出用の倍率を掛けて反映する。判定の大きさは変わらないように割り戻す</summary>
	void UpdateScale();
	/// <summary>向いている左右へ、補間しながら振り向く</summary>
	void UpdateFacingRotate(float deltaTime);
	/// <summary>今の状態に合ったアニメーションへ切り替える</summary>
	void UpdateAnimation(float deltaTime);

private:

	// 調整項目
	PlayerParameter parameter_;

	// コンポーネント
	PlayerInput input_;
	PlayerJump  jump_;
	BlockGroupConnectState blockGroupConnectState_;
	BlockGroupLauncherManager blockGroupLauncherManager_;

	// 集合・打ち上げの対象を引き当てる連結グループ表(非所有)
	StageBlockField* pBlockField_ = nullptr;

	// 最後に向いた左右方向
	float facing_ = 1.0f;

	// 見た目
	// 演出でスケールを動かす時の倍率。baseScaleに掛けて使う
	Math::Vector3 scaleMultiplier_ = CVector3::UNIT;
	// 左右入力が無い状態が続いている時間。切り返しの一瞬でidleへ落とさないために見る
	float noMoveInputTimer_ = 0.0f;

	// 被弾
	float currentHp_ = 0.0f;			// 現在のHP
	float invincibleTimer_ = 0.0f;		// 残りの無敵時間
	float invincibleBlinkTimer_ = 0.0f;	// 点滅の切り替え用タイマー
	bool isBlinkVisible_ = true;		// 点滅の表示状態

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
	// 足元判定で、自分が乗っているマスを拾わないように空ける隙間
	static constexpr float kGroundCheckEpsilon = 0.001f;
	// ブロック1個の高さの半分。上面の高さを出すのに使う
	static constexpr float kBlockHalfHeight = 0.5f;

	// モデルに入っているアニメーション名
	static inline const std::string kIdleAnimation = "idle";
	static inline const std::string kWalkAnimation = "walk";
	static inline const std::string kJumpAnimation = "jump";

public: // accessor
	const BlockGroupLauncherManager* GetBlockGroupLauncherManager() const{ return &blockGroupLauncherManager_; }
	BlockGroupLauncherManager* GetBlockGroupLauncherManagerRef(){ return &blockGroupLauncherManager_; }

	bool IsGrounded() const{ return jump_.IsGrounded(); }

	// 落下中（着地しうる状態）かどうか
	bool IsFalling() const{ return jump_.GetState() == PlayerJump::State::Falling || jump_.GetState() == PlayerJump::State::Hanging; }

	// 着地したブロックグループを接続対象へ追加する。追加できたグループは色が変わる
	bool TryConnectBlockGroup(int groupId);

	// ダメージ床に当たった時のノックバックを開始する
	void ApplyDamageFloorKnockback(float power);

	/// <summary>
	/// ダメージを受ける。無敵中は何も起きない
	/// </summary>
	/// <returns>実際にダメージが入ったら true</returns>
	bool TakeDamage(float amount);

	float GetMaxHp() const { return parameter_.maxHp; }
	float GetCurrentHp() const { return currentHp_; }
	bool IsInvincible() const { return invincibleTimer_ > 0.0f; }
	bool IsDead() const { return currentHp_ <= 0.0f; }
	// ダメージ床で打ち上げられてから着地するまでの間か
	bool IsDamageFloorAirborne() const { return damageFloorAirborne_; }

	// 集合・打ち上げでグループを引き当てるための連結グループ表を設定する(非所有)
	void SetBlockField(StageBlockField* field);

	// ステージが作り直される時に、保持しているブロックへの参照を全て手放す
	void ResetStageReferences();

	Math::Vector3 GetVelocity() const;
	float GetFacing() const{ return facing_; }

	// 演出でスケールを動かす時の倍率。基準の大きさ(baseScale)に掛かる
	void SetScaleMultiplier(const Math::Vector3& multiplier){ scaleMultiplier_ = multiplier; }
	const Math::Vector3& GetScaleMultiplier() const{ return scaleMultiplier_; }
	const Math::Vector3& GetBaseScale() const{ return parameter_.baseScale; }
};

#pragma once
#include <string>
#include <vector>
#include "Engine/Module/Components/GameObject/BaseEntity.h"
#include "Engine/Lib/Math/Vector3.h"
#include "Engine/Lib/Math/Quaternion.h"
#include "Game/Actor/Player/PlayerParameter.h"
#include "Game/Actor/Player/Component/PlayerAnimation.h"
#include "Game/Actor/Player/Component/PlayerBlockIgnore.h"
#include "Game/Actor/Player/Component/PlayerGroundState.h"
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
	/// <summary>接地判定の結果を、ジャンプの状態へ反映する。</summary>
	void ResolveGround(float deltaTime);
	/// <summary>接地判定・位置補正のコンポーネントへ渡す今の状況を作る。</summary>
	PlayerGroundState::Context MakeGroundContext() const;
	/// <summary>接地判定・位置補正のコンポーネントへ渡す調整値を作る。</summary>
	PlayerGroundState::Params MakeGroundParams() const;
	/// <summary>Blockの一時無効化コンポーネントへ渡す今の状況を作る。</summary>
	PlayerBlockIgnore::Context MakeBlockIgnoreContext() const;
	/// <summary>無敵時間を進めつつ、その間ちかちか点滅させる</summary>
	void UpdateInvincible(float deltaTime);
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
	PlayerAnimation animation_;
	PlayerGroundState groundState_;
	PlayerBlockIgnore blockIgnore_;
	BlockGroupConnectState blockGroupConnectState_;
	BlockGroupLauncherManager blockGroupLauncherManager_;

	// 集合・打ち上げの対象を引き当てる連結グループ表(非所有)
	StageBlockField* pBlockField_ = nullptr;

	// 最後に向いた左右方向
	float facing_ = 1.0f;

	// 見た目
	// 演出でスケールを動かす時の倍率。baseScaleに掛けて使う
	Math::Vector3 scaleMultiplier_ = CVector3::UNIT;

	// 被弾
	float currentHp_ = 0.0f;			// 現在のHP
	float invincibleTimer_ = 0.0f;		// 残りの無敵時間
	float invincibleBlinkTimer_ = 0.0f;	// 点滅の切り替え用タイマー
	bool isBlinkVisible_ = true;		// 点滅の表示状態

	// ダメージ床で打ち上げられてから着地するまでtrue
	bool damageFloorAirborne_ = false;

	// 自分のCollider category名
	static inline const std::string kColliderTag = "Player";

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

	// 集合・打ち上げでグループを引き当てるための連結グループ表を設定する
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

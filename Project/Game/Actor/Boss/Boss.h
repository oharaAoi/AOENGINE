#pragma once
#include "Engine/Module/Components/GameObject/BaseEntity.h"
#include <cstdint>
#include <string>
#include "Engine/Lib/Math/Vector3.h"
#include "Engine/Lib/Math/Matrix4x4.h"
#include "Game/Actor/Boss/BossParameter.h"
#include "Game/Actor/Common/ScreenWorldPlaneAnchor.h"
#include "Game/Actor/Boss/Component/BossAnimation.h"
#include "Game/Actor/Boss/Component/BossCollision.h"
#include "Game/Actor/Boss/Component/BossDamageEffect.h"
#include "Game/Actor/Boss/BossBehaviorController.h"

namespace AOENGINE {
	class BaseParticles;
}

class StageBlockField;
class FollowCamera;

/// <summary>
/// ボス
/// </summary>
class Boss : public AOENGINE::BaseEntity {
public:

	Boss() = default;
	~Boss() override = default;
	Boss(const Boss&) = delete;
	Boss& operator=(const Boss&) = delete;

	// 初期化、更新
	void Init(AOENGINE::BaseGameObject* body);
	void Update(const Math::Matrix4x4& viewProjection);

	/// <summary>
	/// 行動もアニメーションも進めず、画面上の位置と大きさだけ合わせる。
	/// イントロのように、映したいが動かしたくない時に使う
	/// </summary>
	void UpdateStandby(const Math::Matrix4x4& viewProjection, float deltaTime);

	/// <summary>
	/// 登場の降下を始める。定位置の上から降りてきて、着いたら揺れる
	/// </summary>
	void StartIntroDescend();

	/// <summary>攻撃に入った合図のエフェクトを、決めた数だけ出し始める</summary>
	void PlayAttackEffect();

	/// <summary>攻撃を出した合図に、大きさを一度だけ膨らませる</summary>
	void StartAttackPulse();

	/// <summary>パルスを打ち切って元の大きさへ戻す</summary>
	void StopAttackPulse();

	void Debug_Gui();

private:

	/// <summary>基準スケールに演出用の倍率を掛けて反映する。判定の大きさは変わらないように割り戻す</summary>
	void UpdateScale();
	/// <summary>今の行動に合わせてアニメーションを進める</summary>
	void UpdateAnimation();

	/// <summary>流すアニメーションを指定して進める</summary>
	void UpdateAnimation(const std::string& animationName);

	/// <summary>被弾の演出に渡す調整値をまとめる</summary>
	BossDamageEffect::Params MakeDamageEffectParams() const;

	/// <summary>降下を進めて、定位置からのずらし量を更新する</summary>
	void UpdateIntroDescend(float deltaTime);

	/// <summary>攻撃のパルスを進めて、大きさの倍率へ反映する</summary>
	void UpdateAttackPulse(float deltaTime);

	/// <summary>残っている発数を、間隔を空けながら出していく</summary>
	void UpdateAttackEffect(float deltaTime);

	/// <summary>エフェクトを1発ぶん出す</summary>
	void EmitAttackEffect();

	/// <summary>全体の進み具合を、パルス1回ぶんの進み具合へ直す</summary>
	float CalcPulseLocalRatio(float ratio) const;

public:

	// ダメージを受ける
	void Damage(float amount);

private:
	// bossBehavior
	BossBehaviorController behaviorController_;

	// 行動に合わせてアニメーションを切り替える
	BossAnimation animation_;

	// 被弾した時に色を変えて揺らす
	BossDamageEffect damageEffect_;

	// 攻撃に入った時に出すエフェクト
	AOENGINE::BaseParticles* attackEffect_ = nullptr;
	// まだ出していない発数と、次の1発までの経過時間
	int32_t attackEffectRemaining_ = 0;
	float attackEffectTimer_ = 0.0f;

	// 攻撃を出した時のパルス
	bool isAttackPulsing_ = false;
	float attackPulseTimer_ = 0.0f;

	// 登場の降下
	bool isIntroDescending_ = false;
	float introDescendTimer_ = 0.0f;
	// 定位置からのずらし量。降りきると0になる
	float introDescendOffsetY_ = 0.0f;

	// パラメータ、位置固定などのなどComponent
	BossParameter parameter_;
	ScreenWorldPlaneAnchor screenAnchor_;

	Math::Vector3 position_{};
	float currentHp_ = 0.0f;

	// 演出でスケールを動かす時の倍率。baseScaleに掛けて使う
	Math::Vector3 scaleMultiplier_ = CVector3::UNIT;

	// ダメージを受け付けない状態か
	bool isInvincible_ = false;

	// 撃破の演出まで終わったか
	bool isDefeatFinished_ = false;

	// 自分のCollider category名
	const std::string kColliderTag = "Boss";

	// 行動を進めない間に流しておくアニメーション名
	const std::string kStandbyAnimationName = "idle";

	// 攻撃に入った時に出すエフェクトの名前
	const std::string kAttackEffectName = "BossAttackEffect";

	// 落とす足場を選ぶためのブロックの表
	StageBlockField* pBlockField_ = nullptr;

	// 揺らす対象のカメラ
	FollowCamera* pCamera_ = nullptr;

	// 直近のviewProjection
	Math::Matrix4x4 viewProjection_{};

public: // accessor

	float GetMaxHp() const { return parameter_.hp; }
	float GetCurrentHp() const { return currentHp_; }
	bool IsDefeated() const { return currentHp_ <= 0.0f; }

	// 撃破の演出まで終わったか。シーンをクリアへ移す合図に使う
	void SetDefeatFinished(bool isFinished) { isDefeatFinished_ = isFinished; }
	bool IsDefeatFinished() const { return isDefeatFinished_; }

	/// <summary>登場の降下が終わっているか。始めていない場合も終わり扱いにする</summary>
	bool IsIntroDescendFinished() const { return !isIntroDescending_; }

	// フェーズ切り替えの演出中など、ダメージを受け付けない状態にする
	void SetInvincible(bool isInvincible) { isInvincible_ = isInvincible; }
	bool IsInvincible() const { return isInvincible_; }
	const Math::Vector3& GetPosition() const { return position_; }

	/// <summary>攻撃行動側から調整値を参照するために公開する</summary>
	const BossParameter& GetParameter() const { return parameter_; }

	/// <summary>
	/// 残HPの割合から今のフェーズ番号を求める
	/// </summary>
	int32_t GetPhaseIndex() const;

	/// <summary>
	/// 指定したワールド座標が画面の上端より下か。
	/// </summary>
	bool IsBelowCameraTop(const Math::Vector3& worldPosition) const;

	/// <summary>
	/// 指定したワールド座標がカメラに映る範囲に入っているか。
	/// </summary>
	bool IsInCameraView(const Math::Vector3& worldPosition) const;

	/// <summary>
	/// 指定したワールド座標が画面の下端より下へ抜けたか。
	/// </summary>
	bool IsBelowCameraBottom(const Math::Vector3& worldPosition) const;

	/// <summary>攻撃側から、同じアニメーションを頭から流し直す</summary>
	void ReplayAnimation() { animation_.Replay(); }

	/// <summary>攻撃側からカメラを揺らす。カメラが未設定なら何もしない</summary>
	void ShakeCamera(const CameraShakeRequest& request);

	/// <summary>カメラを揺らせるように渡しておく</summary>
	void SetCamera(FollowCamera* camera) { pCamera_ = camera; }

	/// <summary>足止めを落とす足場を選ぶために、ブロックの表を渡しておく</summary>
	void SetBlockField(StageBlockField* field) { pBlockField_ = field; }
	StageBlockField* GetBlockField() const { return pBlockField_; }

	// 演出でスケールを動かす時の倍率。基準の大きさに掛かる
	void SetScaleMultiplier(const Math::Vector3& multiplier) { scaleMultiplier_ = multiplier; }
	const Math::Vector3& GetScaleMultiplier() const { return scaleMultiplier_; }
	const Math::Vector3& GetBaseScale() const { return parameter_.baseScale; }

};

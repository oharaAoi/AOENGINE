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
#include "Game/Actor/Boss/BossBehaviorController.h"

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

	void Debug_Gui();

private:

	/// <summary>基準スケールに演出用の倍率を掛けて反映する。判定の大きさは変わらないように割り戻す</summary>
	void UpdateScale();
	/// <summary>今の行動に合わせてアニメーションを進める</summary>
	void UpdateAnimation();

public:

	// ダメージを受ける
	void Damage(float amount);

private:
	// bossBehavior
	BossBehaviorController behaviorController_;

	// 行動に合わせてアニメーションを切り替える
	BossAnimation animation_;

	// パラメータ、位置固定などのなどComponent
	BossParameter parameter_;
	ScreenWorldPlaneAnchor screenAnchor_;

	Math::Vector3 position_{};
	float currentHp_ = 0.0f;

	// 演出でスケールを動かす時の倍率。baseScaleに掛けて使う
	Math::Vector3 scaleMultiplier_ = CVector3::UNIT;

	// 自分のCollider category名
	static inline const std::string kColliderTag = "Boss";

	// 落とす足場を選ぶためのブロックの表
	StageBlockField* pBlockField_ = nullptr;

	// 揺らす対象のカメラ
	FollowCamera* pCamera_ = nullptr;

	// 直近のviewProjection
	Math::Matrix4x4 viewProjection_{};

public: // accessor

	float GetMaxHp() const { return parameter_.hp; }
	float GetCurrentHp() const { return currentHp_; }
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

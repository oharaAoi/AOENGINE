#pragma once
#include "Engine/Module/Components/GameObject/BaseEntity.h"
#include "Engine/Lib/Math/Vector3.h"
#include "Engine/Lib/Math/Matrix4x4.h"
#include "Game/Actor/Boss/BossParameter.h"
#include "Game/Actor/Common/ScreenWorldPlaneAnchor.h"
#include "Game/Actor/Boss/Component/BossCollision.h"
#include "Game/Actor/Boss/BossBehaviorController.h"

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

	// ダメージを受ける
	void Damage(float amount);

private:
	// bossBehavior
	BossBehaviorController behaviorController_;

	// パラメータ、位置固定などのなどComponent
	BossParameter parameter_;
	ScreenWorldPlaneAnchor screenAnchor_;

	Math::Vector3 position_{};
	float currentHp_ = 0.0f;

public: // accessor

	float GetMaxHp() const { return parameter_.hp; }
	float GetCurrentHp() const { return currentHp_; }
	const Math::Vector3& GetPosition() const { return position_; }

	/// <summary>攻撃行動側から調整値を参照するために公開する</summary>
	const BossParameter& GetParameter() const { return parameter_; }

};

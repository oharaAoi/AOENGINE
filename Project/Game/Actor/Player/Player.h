#pragma once
// Engine
#include "Engine/Module/Components/GameObject/BaseEntity.h"
#include "Engine/Lib/Math/Vector2.h"
#include <Module/Components/Collider/SphereCollider.h>
#include "Engine/Module/Components/Rigging/EndEffector.h"
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/Utilities/Timer.h"
#include "Engine/Module/Components/Animation/VectorTween.h"
#include "Engine/Module/PostEffect/Vignette.h"
// Game
#include "Game/Camera/FollowCamera.h"
#include "Game/State/StateMachine.h"
#include "Game/Manager/ActionManager.h"
#include "Game/Actor/Player/Bullet/PlayerBulletManager.h"
#include "Game/UI/Reticle.h"
// Weapon
#include "Game/Actor/Weapon/BaseWeapon.h"
#include "Game/Actor/Jet/JetEngine.h"

enum PlayerWeapon {
	Left_Weapon,
	Right_Weapon,
	Left_Shoulder,
	Right_Shoulder,
	KMax
};

/// <summary>
/// Playerクラス
/// </summary>
class Player :
	public AOENGINE::BaseEntity {
public:		// data

	struct Parameter : public AOENGINE::IJsonConverter {
		float health;				// hp
		float postureStability;		// 姿勢安定
		float psRecoveryTime = 3.0f;// 姿勢安定回復時間
		float psRecoveryValue = 3.0f;// 姿勢安定回復量
		float bodyWeight = 1.0f;	// 機体の重さ

		float energy = 1.f;	// EN出力
		float energyRecoveyAmount = 1.f;	// EN回復量(m/s)
		float energyRecoveyCoolTime;	// EN回復までのクールタイム

		float legColliderRadius = 0.5f;
		float legColliderPosY = -0.1f;

		float windDrag;	// 空気抵抗量

		float inclineStrength = 0.1f;		// 傾きの強さ
		float inclineReactionRate = 5.0f;	// 傾きの反応速度
		float inclineThreshold = 10.0f;

		float invincibleTime = 1.0f;				// 無敵時間

		float pinchOfPercentage = 0.3f;			// ピンチの割合
		float pinchVignettePower;
		AOENGINE::Color pinchVignetteColor;

		Math::Vector3 cameraOffset = CVector3::ZERO;
		Math::Vector3 translateOffset = CVector3::ZERO;

		Parameter() {
			SetGroupName("Player");
			SetName("playerParameter");
		}

		json ToJson(const std::string& id) const override {
			return AOENGINE::JsonBuilder(id)
				.Add("health", health)
				.Add("postureStability", postureStability)
				.Add("psRecoveryTime", psRecoveryTime)
				.Add("psRecoveryValue", psRecoveryValue)
				.Add("bodyWeight", bodyWeight)
				.Add("energy", energy)
				.Add("energyRecoveyAmount", energyRecoveyAmount)
				.Add("energyRecoveyCoolTime", energyRecoveyCoolTime)
				.Add("legColliderRadius", legColliderRadius)
				.Add("legColliderPosY", legColliderPosY)
				.Add("windDrag", windDrag)
				.Add("inclineStrength", inclineStrength)
				.Add("inclineReactionRate", inclineReactionRate)
				.Add("inclineThreshold", inclineThreshold)
				.Add("cameraOffset", cameraOffset)
				.Add("translateOffset", translateOffset)
				.Add("pinchOfPercentage", pinchOfPercentage)
				.Add("pinchVignettePower", pinchVignettePower)
				.Add("pinchVignetteColor", pinchVignetteColor)
				.Add("invincibleTime", invincibleTime)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			Convert::fromJson(jsonData, "health", health);
			Convert::fromJson(jsonData, "postureStability", postureStability);
			Convert::fromJson(jsonData, "psRecoveryTime", psRecoveryTime);
			Convert::fromJson(jsonData, "psRecoveryValue", psRecoveryValue);
			Convert::fromJson(jsonData, "bodyWeight", bodyWeight);
			Convert::fromJson(jsonData, "energy", energy);
			Convert::fromJson(jsonData, "energyRecoveyAmount", energyRecoveyAmount);
			Convert::fromJson(jsonData, "energyRecoveyCoolTime", energyRecoveyCoolTime);
			Convert::fromJson(jsonData, "legColliderRadius", legColliderRadius);
			Convert::fromJson(jsonData, "legColliderPosY", legColliderPosY);
			Convert::fromJson(jsonData, "windDrag", windDrag);
			Convert::fromJson(jsonData, "inclineStrength", inclineStrength);
			Convert::fromJson(jsonData, "inclineReactionRate", inclineReactionRate);
			Convert::fromJson(jsonData, "inclineThreshold", inclineThreshold);
			Convert::fromJson(jsonData, "cameraOffset", cameraOffset);
			Convert::fromJson(jsonData, "translateOffset", translateOffset);
			Convert::fromJson(jsonData, "pinchOfPercentage", pinchOfPercentage);
			Convert::fromJson(jsonData, "pinchVignettePower", pinchVignettePower);
			Convert::fromJson(jsonData, "pinchVignetteColor", pinchVignetteColor);
			Convert::fromJson(jsonData, "invincibleTime", invincibleTime);
		}

		void Debug_Gui() override;
	};

public:	// コンストラクタ

	Player();
	~Player();

public:

	// 終了処理
	void Finalize();
	// 初期化
	void Init();
	// 更新
	void Update();
	// 編集
	void Debug_Gui() override;
	// 後から更新
	void PosUpdate();

public:	// member method

	/// <summary>
	/// targetの方向を見る
	/// </summary>
	void LookTarget(float _rotateT_, bool isLockOn);

	/// <summary>
	/// 攻撃を行う
	/// </summary>
	void Attack(PlayerWeapon _weapon, AttackContext _contex);

	/// <summary>
	/// jointの座標の更新
	/// </summary>
	void UpdateJoint();

	/// <summary>
	/// ノックバック処理
	/// </summary>
	/// <param name="direction"></param>
	void Knockback(const Math::Vector3& direction);

	/// <summary>
	/// エネルギー回復処理
	/// </summary>
	/// <param name="timer"></param>
	void RecoveryEN(float timer);

	/// <summary>
	/// エネルギー消費処理
	/// </summary>
	/// <param name="cousumeAmount"></param>
	void ConsumeEN(float cousumeAmount);

	/// <summary>
	/// Damageを与える
	/// </summary>
	/// <param name="_damage"></param>
	void Damage(float _damage);

	/// <summary>
	/// 着地した時
	/// </summary>
	void Landing();

	/// <summary>
	/// BoostModeにする
	/// </summary>
	void IsBoostMode();

	/// <summary>
	/// 足のコリジョン
	/// </summary>
	/// <param name="other"></param>
	void LegOnCollision([[maybe_unused]] AOENGINE::BaseCollider* other);

	bool IsAttack();

private:

	// カメラの傾きを行う
	void CameraIncline();

	// 姿勢安定回復
	void PostureStabilityRecovery();

public: // accessor method

	void SetKnockback(bool knockback) { isKnockback_ = knockback; }
	bool GetKnockBack() { return isKnockback_; }

	const Math::Vector3& GetKnockBackDire() const { return knockBackDire_; }

	// 着地したかのフラグ
	bool GetIsLanding() const { return isLanding_; }
	void SetIsLanding(bool _landing) { isLanding_ = _landing; }

	// 移動しているかのフラグ
	bool GetIsMoving() const { return isMoving_; }
	void SetIsMoving(bool isMoving) { isMoving_ = isMoving; }

	// parameter
	Parameter& GetParam() { return param_; }
	const Parameter& GetInitParam() { return initParam_; }

	void SetPostureStability(float _postureStability) { param_.postureStability = _postureStability; }

	// jet
	AOENGINE::BaseGameObject* GetJet() { return jet_->GetGameObject(); }

	// stateMachine
	StateMachine<Player>* GetState() { return stateMachine_.get(); }

	// actionManager
	ActionManager<Player>* GetActionManager() { return actionManager_.get(); }

	// camera
	void SetFollowCamera(FollowCamera* followCamera) { pFollowCamera_ = followCamera; }
	FollowCamera* GetFollowCamera() { return pFollowCamera_; }

	// reticle
	void SetReticle(Reticle* reticle) { reticle_ = reticle; }
	Reticle* GetReticle() const { return reticle_; }
	bool GetIsLockOn() const { return reticle_->GetLockOn(); }

	// targetの座標を
	Math::Vector3 GetTargetPos() const { return reticle_->GetTargetPos(); }

	// bullet
	void SetBulletManager(PlayerBulletManager* bulletManager) { pBulletManager_ = bulletManager; }

	// weapon
	void SetWeapon(BaseWeapon* _weapon, PlayerWeapon _type);
	BaseWeapon* GetWeapon(PlayerWeapon _type) { return pWeapons_[_type]; }

	JetEngine* GetJetEngine() { return jet_.get(); }
	bool GetIsBoostMode() const { return jet_->GetIsBoostMode(); }

	void SetDeployArmor(bool _deployArmor) { deployArmor_ = _deployArmor; }
	bool GetIsDeployArmor() const { return deployArmor_; }

	void SetIsDead(bool _isDead) { isDead_ = _isDead; }
	bool GetIsDead() const { return isDead_; }

	void SetIsExpload(bool _isExpload) { isExpload_ = _isExpload; }
	bool GetIsExpload() const { return isExpload_; }

	const Math::Matrix4x4& GetLeftHandMat() { return leftHandMat_; }
	const Math::Matrix4x4& GetLeftShoulderMat() { return leftShoulderMat_; }
	const Math::Matrix4x4& GetRightHandMat() { return rightHandMat_; }
	const Math::Matrix4x4& GetRightShoulderMat() { return rightShoulderMat_; }

private:

	// 他クラス ------------------------------------------------

	FollowCamera* pFollowCamera_ = nullptr;

	Reticle* reticle_ = nullptr;

	PlayerBulletManager* pBulletManager_ = nullptr;

	// state --------------------------------------------------
	std::unique_ptr<StateMachine<Player>> stateMachine_;

	std::unique_ptr<ActionManager<Player>> actionManager_;

	bool isKnockback_;
	Math::Vector3 knockBackDire_;

	bool isLanding_;
	bool isMoving_;
	bool deployArmor_;
	bool isDead_ = false;
	bool isAttack_ = false;
	bool isExpload_ = false;

	// 無敵時間
	bool isInvincible_ = false;
	AOENGINE::Timer invincibleTimer_;

	// Parameter --------------------------------------------------
	// 姿勢安定ゲージ
	Parameter param_;
	Parameter initParam_;

	AOENGINE::Timer psRecoveryTimer_;

	AOENGINE::VectorTween<float> vignetteTween_;
	std::shared_ptr<PostEffect::Vignette> vignette_;

	// weapon --------------------------------------------------

	BaseWeapon* pWeapons_[PlayerWeapon::KMax];
	
	std::unique_ptr<JetEngine> jet_;

	std::unique_ptr<AOENGINE::SphereCollider> legCollider_;
	
	Math::Matrix4x4 leftHandMat_;
	Math::Matrix4x4 rightHandMat_;
	Math::Matrix4x4 leftShoulderMat_;
	Math::Matrix4x4 rightShoulderMat_;

	Math::Vector2 screenPos_;
	Math::Vector2 screenPosPrev_;
	float smoothedDiffX_ = 0.0f;

	std::deque<std::pair<PlayerWeapon, AttackContext>> attackHistory_;

	// IK --------------------------------------------------
	std::unique_ptr<EndEffector> leftLegEffector_;
	std::unique_ptr<EndEffector> rightLegEffector_;
};


#pragma once
// c++
#include <unordered_map>
// Engine
#include "Engine/Module/Components/GameObject/BaseEntity.h"
#include "Engine/Module/Components/Collider/SphereCollider.h"
#include "Engine/Module/Components/Rigging/EndEffector.h"
#include "Engine/Module/Components/Animation/VectorTween.h"
#include "Engine/Module/PostEffect/Vignette.h"
#include "Engine/Lib/Math/Vector2.h"
#include "Engine/Utilities/Timer.h"
// Game
#include "Game/Camera/FollowCamera.h"
#include "Game/State/StateMachine.h"
#include "Game/Manager/ActionManager.h"
#include "Game/Actor/Player/Bullet/PlayerBulletManager.h"
#include "Game/Actor/Player/PlayerParameter.h"
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

	/// <summary>
	/// Objectの初期化
	/// </summary>
	void InitObject();

	/// <summary>
	/// Action関連の初期化
	/// </summary>
	void InitAction();

	/// <summary>
	/// Colliderの初期化
	/// </summary>
	void InitCollider();

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
	PlayerParameter& GetParam() { return param_; }
	const PlayerParameter& GetInitParam() { return initParam_; }

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

	void SetIsExplode(bool _isExplode) { isExplode_ = _isExplode; }
	bool GetIsExplode() const { return isExplode_; }

	const Math::Matrix4x4& GetWeaponBornMatrix(PlayerWeapon type) { return weaponBornMatrix_[type]; }

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
	bool isExplode_ = false;

	// 無敵時間
	bool isInvincible_ = false;
	AOENGINE::Timer invincibleTimer_;

	// Parameter --------------------------------------------------
	// 姿勢安定ゲージ
	PlayerParameter param_;
	PlayerParameter initParam_;

	AOENGINE::Timer psRecoveryTimer_;

	AOENGINE::VectorTween<float> vignetteTween_;
	std::shared_ptr<PostEffect::Vignette> vignette_;

	// weapon --------------------------------------------------

	BaseWeapon* pWeapons_[PlayerWeapon::KMax];
	
	std::unique_ptr<JetEngine> jet_;

	std::unique_ptr<AOENGINE::SphereCollider> legCollider_;
	
	std::unordered_map<PlayerWeapon, Math::Matrix4x4> weaponBornMatrix_;

	Math::Vector2 screenPos_;
	Math::Vector2 screenPosPrev_;
	float smoothedDiffX_ = 0.0f;

	std::deque<std::pair<PlayerWeapon, AttackContext>> attackHistory_;

	// IK --------------------------------------------------
	std::unique_ptr<EndEffector> leftLegEffector_;
	std::unique_ptr<EndEffector> rightLegEffector_;
};


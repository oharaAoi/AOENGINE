#pragma once
// Engine
#include "Engine/Module/Components/GameObject/BaseEntity.h"
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
	LEFT_WEAPON,
	RIGHT_WEAPON,
	KMAX
};

class Player :
	public BaseEntity {
public:		// data

	struct Parameter : public IJsonConverter {
		float health;				// hp
		float postureStability;		// 姿勢安定
		float bodyWeight = 1.0f;	// 機体の重さ

		float energy = 1.f;	// EN出力
		float energyRecoveyAmount = 1.f;	// EN回復量(m/s)
		float energyRecoveyCoolTime;	// EN回復までのクールタイム

		float legColliderRadius = 0.5f;
		float legColliderPosY = -0.1f;

		json ToJson(const std::string& id) const override {
			return JsonBuilder(id)
				.Add("health", health)
				.Add("postureStability", postureStability)
				.Add("bodyWeight", bodyWeight)
				.Add("energy", energy)
				.Add("energyRecoveyAmount", energyRecoveyAmount)
				.Add("energyRecoveyCoolTime", energyRecoveyCoolTime)
				.Add("legColliderRadius", legColliderRadius)
				.Add("legColliderPosY", legColliderPosY)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			fromJson(jsonData, "health", health);
			fromJson(jsonData, "postureStability", postureStability);
			fromJson(jsonData, "bodyWeight", bodyWeight);
			fromJson(jsonData, "energy", energy);
			fromJson(jsonData, "energyRecoveyAmount", energyRecoveyAmount);
			fromJson(jsonData, "energyRecoveyCoolTime", energyRecoveyCoolTime);
			fromJson(jsonData, "legColliderRadius", legColliderRadius);
			fromJson(jsonData, "legColliderPosY", legColliderPosY);
		}
	};

public:		// base

	Player();
	~Player();

	void Finalize();
	void Init();
	void Update();
	void Draw() const;

	void Debug_Gui() override;

public:		// accessor method

	void SetKnockback(bool knockback) { isKnockback_ = knockback; }
	bool GetKnockBack() { return isKnockback_; }

	const Vector3& GetKnockBackDire() const { return knockBackDire_; }

	/// <summary>
	/// ノックバック処理
	/// </summary>
	/// <param name="direction"></param>
	void Knockback(const Vector3& direction);

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
	/// 着地した時
	/// </summary>
	void Landing();

	/// <summary>
	/// BoostModeにする
	/// </summary>
	void IsBoostMode();

	void LegOnCollision([[maybe_unused]] ICollider* other);

public:

	// 着地したかのフラグ
	bool GetIsLanding() const { return isLanding_; }
	void SetIsLanding(bool _landing) { isLanding_ = _landing; }

	// 移動しているかのフラグ
	bool GetIsMoving() const { return isMoving_; }
	void SetIsMoving(bool isMoving) { isMoving_ = isMoving; }

	// parameter
	Parameter& GetParam() { return param_; }
	const Parameter& GetInitParam() { return initParam_; }

	// jet
	BaseGameObject* GetJet() { return jet_->GetGameObject(); }

	// stateMachine
	StateMachine<Player>* GetState() { return stateMachine_.get(); }

	// actionManager
	ActionManager<Player>* GetActionManager() { return actionManager_.get(); }

	// camera
	void SetFollowCamera(FollowCamera* followCamera) { pFollowCamera_ = followCamera; }
	FollowCamera* GetFollowCamera() { return pFollowCamera_; }

	// reticle
	void SetReticle(Reticle* reticle) { reticle_ = reticle; }

	// targetの座標を
	Vector3 GetTargetPos() const { return reticle_->GetTargetPos(); }

	// bullet
	void SetBulletManager(PlayerBulletManager* bulletManager) { pBulletManager_ = bulletManager; }

	// weapon
	void SetWeapon(BaseWeapon* _weapon, PlayerWeapon _type);
	BaseWeapon* GetWeapon(PlayerWeapon _type) { return pWeapons_[_type]; }

	JetEngine* GetJetEngine() { return jet_.get(); }
	bool GetIsBoostMode() const { return jet_->GetIsBoostMode(); }

private:

	// 他クラス ------------------------------------------------

	FollowCamera* pFollowCamera_ = nullptr;

	Reticle* reticle_ = nullptr;

	PlayerBulletManager* pBulletManager_ = nullptr;

	// state --------------------------------------------------
	std::unique_ptr<StateMachine<Player>> stateMachine_;

	std::unique_ptr<ActionManager<Player>> actionManager_;

	bool isKnockback_;
	Vector3 knockBackDire_;

	bool isLanding_;

	bool isMoving_;

	// Parameter --------------------------------------------------
	// 姿勢安定ゲージ
	Parameter param_;
	Parameter initParam_;

	// weapon --------------------------------------------------

	BaseWeapon* pWeapons_[PlayerWeapon::KMAX];
	
	std::unique_ptr<JetEngine> jet_;

	std::unique_ptr<SphereCollider> legCollider_;

};


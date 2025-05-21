#pragma once
// Engine
#include "Engine/Module/Components/GameObject/BaseGameObject.h"
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
	public BaseGameObject {
public:		// data

	struct Parameter : public IJsonConverter {
		float energy = 1.f;	// EN出力
		float energyRecoveyAmount = 1.f;	// EN回復量(m/s)
		float energyRecoveyCoolTime;	// EN回復までのクールタイム

		json ToJson(const std::string& id) const override {
			return JsonBuilder(id)
				.Add("energy", energy)
				.Add("energyRecoveyAmount", energyRecoveyAmount)
				.Add("energyRecoveyCoolTime", energyRecoveyCoolTime)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			fromJson(jsonData, "energy", energy);
			fromJson(jsonData, "energyRecoveyAmount", energyRecoveyAmount);
			fromJson(jsonData, "energyRecoveyCoolTime", energyRecoveyCoolTime);
		}
	};

public:		// base

	Player();
	~Player();

	void Finalize() override;
	void Init() override;
	void Update() override;
	void Draw() const override;

#ifdef _DEBUG
	void Debug_Gui() override;
#endif // _DEBUG

public:		// accessor method

	void SetKnockback(bool knockback) { isKnockback_ = knockback; }
	bool GetKnockBack() { return isKnockback_; }

	const Vector3& GetKnockBackDire() const { return knockBackDire_; }

	void Knockback(const Vector3& direction);

	void RecoveryEN(float timer);

	void ConsumeEN(float cousumeAmount);

	/// <summary>
	/// 着地した時
	/// </summary>
	void Landing();

public:

	// 着地したかのフラグ
	bool GetIsLanding() const { return isLanding_; }
	void SetIsLanding(bool _landing) { isLanding_ = _landing; }
	
	// parameter
	Parameter& GetParam() { return param_; }
	const Parameter& GetInitParam() { return initParam_; }

	// jet
	BaseGameObject* GetJet() { return jet_.get(); }

	// stateMachine
	StateMachine<Player>* GetState() { return stateMachine_.get(); }

	// actionManager
	ActionManager<Player>* GetActionManager() { return actionManager_.get(); }

	// camera
	void SetFollowCamera(FollowCamera* followCamera) { pFollowCamera_ = followCamera; }
	FollowCamera* GetFollowCamera() { return pFollowCamera_; }

	// reticle
	void SetReticle(Reticle* reticle) { reticle_ = reticle; }

	// bullet
	void SetBulletManager(PlayerBulletManager* bulletManager) { pBulletManager_ = bulletManager; }

	// weapon
	void SetWeapon(BaseWeapon* _weapon, PlayerWeapon _type);
	BaseWeapon* GetWeapon(PlayerWeapon _type) { return pWeapons_[_type]; }

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

	// Parameter --------------------------------------------------
	// 姿勢安定ゲージ
	Parameter param_;
	Parameter initParam_;

	// weapon --------------------------------------------------

	BaseWeapon* pWeapons_[PlayerWeapon::KMAX];
	
	std::unique_ptr<JetEngine> jet_;

};


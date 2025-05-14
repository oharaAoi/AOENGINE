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

		json ToJson(const std::string& id) const override {
			return JsonBuilder(id)
				.Add("energy", energy)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			fromJson(jsonData, "energy", energy);
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

	void Knockback();

	void Landing();

	// parameter
	Parameter& GetParam() { return param_; }
	const Parameter& GetInitParam() { return initParam_; }

	// jet
	BaseGameObject* GetJet() { return jet_.get(); }

	// stateMachine
	StateMachine<Player>* GetState() { return stateMachine_.get(); }

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

	ActionManager<Player> actionManager_;

	bool isKnockback_;

	// Parameter --------------------------------------------------
	// 姿勢安定ゲージ
	Parameter param_;
	Parameter initParam_;

	// weapon --------------------------------------------------

	BaseWeapon* pWeapons_[PlayerWeapon::KMAX];
	
	std::unique_ptr<JetEngine> jet_;

};


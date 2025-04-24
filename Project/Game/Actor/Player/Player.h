#pragma once
// Engine
#include "Engine/Components/GameObject/BaseGameObject.h"
// Game
#include "Game/Camera/FollowCamera.h"
#include "Game/State/StateMachine.h"
#include "Game/Manager/ActionManager.h"
#include "Game/Actor/Player/Bullet/PlayerBulletManager.h"
#include "Game/UI/Reticle.h"
#include "Engine/Components/Animation/VectorTween.h"
// Effect
#include "Game/Effects/GunFireParticles.h"

class Player :
	public BaseGameObject {
public:

	Player();
	~Player();

	void Finalize() override;
	void Init() override;
	void Update() override;
	void Draw() const override;

#ifdef _DEBUG
	void Debug_Gui() override;
#endif // _DEBUG

public:

	void Shot(float speed);

public:		// accessor method

	BaseGameObject* GetJet() { return jet_.get(); }

	StateMachine<Player>* GetState() { return stateMachine_.get(); }

	void SetFollowCamera(FollowCamera* followCamera) { pFollowCamera_ = followCamera; }
	FollowCamera* GetFollowCamera() { return pFollowCamera_; }

	void SetReticle(Reticle* reticle) { reticle_ = reticle; }

	void SetBulletManager(PlayerBulletManager* bulletManager) { pBulletManager_ = bulletManager; }

private:

	// 他クラス ------------------------------------------------

	FollowCamera* pFollowCamera_ = nullptr;

	Reticle* reticle_ = nullptr;

	PlayerBulletManager* pBulletManager_ = nullptr;

	// state --------------------------------------------------
	std::unique_ptr<StateMachine<Player>> stateMachine_;

	ActionManager<Player> actionManager_;

	float floatingValue_;
	VectorTween<float> floatingTween_;

	// bullet --------------------------------------------------

	std::unique_ptr<GunFireParticles> gunFireParticles_;

	std::unique_ptr<BaseGameObject> jet_;

};


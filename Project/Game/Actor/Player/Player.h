#pragma once
// Engine
#include "Engine/Components/GameObject/BaseGameObject.h"
// Game
#include "Game/Camera/FollowCamera.h"
#include "Game/State/StateMachine.h"
#include "Game/Manager/ActionManager.h"
#include "Game/Actor/Player/Bullet/PlayerBulletManager.h"

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

	StateMachine<Player>* GetState() { return stateMachine_.get(); }

	void SetFollowCamera(FollowCamera* followCamera) { pFollowCamera_ = followCamera; }
	FollowCamera* GetFollowCamera() { return pFollowCamera_; }

	void SetBulletManager(PlayerBulletManager* bulletManager) { pBulletManager_ = bulletManager; }

private:

	// 他クラス ------------------------------------------------

	FollowCamera* pFollowCamera_ = nullptr;

	PlayerBulletManager* pBulletManager_ = nullptr;

	// state --------------------------------------------------
	std::unique_ptr<StateMachine<Player>> stateMachine_;

	ActionManager<Player> actionManager_;

	// bullet --------------------------------------------------
	
};


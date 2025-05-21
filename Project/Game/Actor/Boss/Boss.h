#pragma once
#include <memory>
// Engine
#include "Engine/Module/Components/GameObject/BaseGameObject.h"
// Game
#include "Game/State/StateMachine.h"
#include "Game/Manager/ActionManager.h"
#include "Game/Actor/Boss/Bullet/BossBulletManager.h"

class Boss :
	public BaseGameObject {
public:

	Boss() = default;
	~Boss() = default;

	void Finalize() override;
	void Init() override;
	void Update() override;
	void Draw() const override;

#ifdef _DEBUG
	void Debug_Gui() override;
#endif // _DEBUG

public:

	StateMachine<Boss>* GetState() { return stateMachine_.get(); }

	void SetPlayerPosition(const Vector3& _position) { playerPosition_ = _position; }
	const Vector3& GetPlayerPosition() const { return playerPosition_; }

	void SetBulletManager(BossBulletManager* _manager) { pBossBulletManager_ = _manager; }
	BossBulletManager* GetBulletManager() { return pBossBulletManager_; }

private:

	// ポインタ  --------------------------------------------------
	BossBulletManager* pBossBulletManager_ = nullptr;

	// state --------------------------------------------------
	std::unique_ptr<StateMachine<Boss>> stateMachine_;

	std::unique_ptr<ActionManager<Boss>> actionManager_;

	// Playerの状態 --------------------------------------------------

	Vector3 playerPosition_;

};


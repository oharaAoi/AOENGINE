#pragma once
#include "Engine/Module/Components/EventHandlers/BaseCollisionCallBacks.h"
#include "Engine/System/Manager/CollisionManager.h"
#include "Game/Actor/Player/Player.h"
#include "Game/WorldObject/Floor.h"

class PlayerToGroundCallBacks :
	public AOENGINE::BaseCollisionCallBacks {
public:

	PlayerToGroundCallBacks(AOENGINE::CollisionManager* _collisionManager) : pCollisionManager_(_collisionManager) {};
	~PlayerToGroundCallBacks() = default;

public:

	// 初期化
	void Init() override;
	// 更新
	void Update() override;

	// 衝突開始
	void CollisionEnter([[maybe_unused]] AOENGINE::BaseCollider* const player, [[maybe_unused]] AOENGINE::BaseCollider* const ground) override;
	// 衝突中
	void CollisionStay([[maybe_unused]] AOENGINE::BaseCollider* const player, [[maybe_unused]] AOENGINE::BaseCollider* const ground)  override;
	// 衝突終わり
	void CollisionExit([[maybe_unused]] AOENGINE::BaseCollider* const player, [[maybe_unused]] AOENGINE::BaseCollider* const ground)  override;

public:		// accessor method

	void SetPlayer(Player* _player) { pPlayer_ = _player; }
	void SetGround(Floor* _floor) { pFloor_ = _floor; }

private:

	AOENGINE::CollisionManager* pCollisionManager_;

	Player* pPlayer_;
	Floor* pFloor_;
};


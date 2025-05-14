#pragma once
#include "Engine/Module/Components/EventHandlers/BaseCollisionCallBacks.h"
#include "Engine/System/Manager/CollisionManager.h"
#include "Game/Actor/Player/Player.h"
#include "Game/WorldObject/Floor.h"

class PlayerToGroundCallBacks :
	public BaseCollisionCallBacks {
public:

	PlayerToGroundCallBacks(CollisionManager* _collisionManager) : pCollisionManager_(_collisionManager) {};
	~PlayerToGroundCallBacks() = default;

	void Init() override;

	void Update() override;

	void CollisionEnter([[maybe_unused]] ICollider* const player, [[maybe_unused]] ICollider* const ground) override;
	void CollisionStay([[maybe_unused]] ICollider* const player, [[maybe_unused]] ICollider* const ground)  override;
	void CollisionExit([[maybe_unused]] ICollider* const player, [[maybe_unused]] ICollider* const ground)  override;

public:		// accessor method

	void SetPlayer(Player* _player) { pPlayer_ = _player; }
	void SetGround(Floor* _floor) { pFloor_ = _floor; }

private:

	CollisionManager* pCollisionManager_;

	Player* pPlayer_;
	Floor* pFloor_;
};


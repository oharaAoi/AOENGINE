#pragma once

/// engine
#include "Engine/System/Manager/CollisionManager.h"
#include "Engine/Module/Components/EventHandlers/BaseCollisionCallBacks.h"

// game
#include "Game/Actor/Player/Player.h"

class DamageFloor;

class PlayerToDamageFloorCallBacks :
	public AOENGINE::BaseCollisionCallBacks {
public: // constructor

	PlayerToDamageFloorCallBacks(AOENGINE::CollisionManager* _collisionManager) : pCollisionManager_(_collisionManager) {};
	~PlayerToDamageFloorCallBacks() override = default;

public: // public method

	void Init() override;
	void Update() override {};

	void CollisionEnter(AOENGINE::BaseCollider* const player, AOENGINE::BaseCollider* const floor) override;
	void CollisionStay(AOENGINE::BaseCollider* const player, AOENGINE::BaseCollider* const floor) override;
	void CollisionExit(AOENGINE::BaseCollider* const player, AOENGINE::BaseCollider* const floor) override;

public: // accessor

	void SetPlayer(Player* player) { pPlayer_ = player; }
	void SetDamageFloor(DamageFloor* damageFloor) { pDamageFloor_ = damageFloor; }

private: // private variable

	Player* pPlayer_ = nullptr;
	DamageFloor* pDamageFloor_ = nullptr;
	AOENGINE::CollisionManager* pCollisionManager_;

};


#pragma once

/// stl
#include <string>

/// engine
#include "Engine/System/Manager/CollisionManager.h"
#include "Engine/Module/Components/EventHandlers/BaseCollisionCallBacks.h"

class Player;
class StageBlockField;

/// <summary>
/// Block / StepBlock がダメージ床に当たった時のコールバック。
/// Block はグループごと、StepBlock は単体で破棄する。
/// </summary>
class BlockToDamageFloorCallBacks :
	public AOENGINE::BaseCollisionCallBacks {
public: // constructor

	BlockToDamageFloorCallBacks(AOENGINE::CollisionManager* _collisionManager) : pCollisionManager_(_collisionManager) {};
	~BlockToDamageFloorCallBacks() override = default;

public: // public method

	void Init() override;
	void Update() override {};

	void CollisionEnter(AOENGINE::BaseCollider* const damageFloor, AOENGINE::BaseCollider* const block) override;
	void CollisionStay(AOENGINE::BaseCollider* const damageFloor, AOENGINE::BaseCollider* const block) override;
	void CollisionExit(AOENGINE::BaseCollider* const damageFloor, AOENGINE::BaseCollider* const block) override;

public: // accessor

	void SetBlockField(StageBlockField* blockField) { pBlockField_ = blockField; }
	void SetPlayer(Player* player) { pPlayer_ = player; }

private: // private method

	/// <summary>当たったColliderからBlock/StepBlockを引き、破壊する</summary>
	void DestroyOnHit(AOENGINE::BaseCollider* const blockCollider);

private: // private variable

	Player* pPlayer_ = nullptr;
	StageBlockField* pBlockField_ = nullptr;
	AOENGINE::CollisionManager* pCollisionManager_ = nullptr;

	// Collider category名(Prefab側の設定と合わせる)
	const std::string kDamageFloorName_ = "DamageFloor";
	const std::string kBlockName_ = "Block";
	const std::string kStepBlockName_ = "StepBlock";

};

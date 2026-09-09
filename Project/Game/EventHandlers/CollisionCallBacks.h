#pragma once

// stl
#include <memory>

// engine
#include "Engine/System/Manager/CollisionManager.h"

// game
#include "Game/EventHandlers/PlayerToDamageFloorCallBacks.h"
#include "Game/EventHandlers/PlayerToBossAttackCallBacks.h"
#include "Game/EventHandlers/StopperToBlockGroupCallBacks.h"
#include "Game/EventHandlers/BlockToDamageFloorCallBacks.h"
#include "Game/Actor/Player/Player.h"

class DamageFloor;
class Boss;
class StageBlockField;

/// <summary>
/// 衝突判定をまとめたクラス
/// </summary>
class CollisionCallBacks {
public: // constructor

	CollisionCallBacks() = default;
	~CollisionCallBacks() = default;

public: // public method

	void Init(AOENGINE::CollisionManager* collisionManager, Player* player, DamageFloor* damageFloor, Boss* boss, StageBlockField* blockField);

private: // private variable

	std::unique_ptr<PlayerToDamageFloorCallBacks> playerToDamageFloor_;
	std::unique_ptr<PlayerToBossAttackCallBacks> playerToBossAttack_;
	std::unique_ptr<StopperToBlockGroupCallBacks> stopperToBlockGroup_;
	std::unique_ptr<BlockToDamageFloorCallBacks> blockToDamageFloor_;

};


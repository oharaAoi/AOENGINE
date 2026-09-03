#pragma once

// stl
#include <memory>

// engine
#include "Engine/System/Manager/CollisionManager.h"

// game
#include "Game/EventHandlers/PlayerToDamageFloorCallBacks.h"
#include "Game/Actor/Player/Player.h"

class DamageFloor;

/// <summary>
/// 衝突判定をまとめたクラス
/// </summary>
class CollisionCallBacks {
public: // constructor

	CollisionCallBacks() = default;
	~CollisionCallBacks() = default;

public: // public method

	void Init(AOENGINE::CollisionManager* collisionManager, Player* player, DamageFloor* damageFloor);

private: // private variable

	std::unique_ptr<PlayerToDamageFloorCallBacks> playerToDamageFloor_;

};


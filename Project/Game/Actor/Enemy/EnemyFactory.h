#pragma once
#include <memory>
#include "Game/Actor/Enemy/BaseEnemy.h"

/// <summary>
/// Enemyを生成するクラス
/// </summary>
class EnemyFactory {
public:

	EnemyFactory() = default;
	~EnemyFactory() = default;

public:

	std::unique_ptr<BaseEnemy> Create(EnemyType type);

};


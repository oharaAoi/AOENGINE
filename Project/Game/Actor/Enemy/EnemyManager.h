#pragma once
#include <list>
#include <memory>
#include "Game/Actor/Enemy/BaseEnemy.h"

/// <summary>
/// Enemyを管理するクラス
/// </summary>
class EnemyManager {
public: // コンストラクタ

	EnemyManager() = default;
	~EnemyManager() = default;

public: // method

	/// <summary>
	/// 初期化処理
	/// </summary>
	void Init();

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

private:

	std::list<std::unique_ptr<BaseEnemy>> enemyList_;


};


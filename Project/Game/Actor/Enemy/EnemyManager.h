#pragma once
#include <list>
#include <memory>
#include "Engine/Module/Components/Attribute/AttributeGui.h"
#include "Game/Actor/Enemy/BaseEnemy.h"
#include "Game/Actor/Enemy/EnemyFactory.h"

/// <summary>
/// Enemyを管理するクラス
/// </summary>
class EnemyManager :
	public AOENGINE::AttributeGui {
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

	/// <summary>
	/// 編集処理
	/// </summary>
	void Debug_Gui() override;

private:

	std::list<std::unique_ptr<BaseEnemy>> enemyList_;

	EnemyFactory enemyFactory_;


};


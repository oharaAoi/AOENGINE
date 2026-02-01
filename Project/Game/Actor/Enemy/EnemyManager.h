#pragma once
#include <list>
#include <memory>
#include "Engine/Module/Components/Attribute/AttributeGui.h"
#include "Game/Actor/Boss/Boss.h"
#include "Game/Actor/Boss/BossRoot.h"
#include "Game/Actor/Enemy/BaseEnemy.h"
#include "Game/Actor/Enemy/EnemyFactory.h"
#include "Game/Actor/Enemy/EnemyBulletManager.h"
#include "Game/Actor/Player/Player.h"

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

	/// <summary>
	/// ボスをリストに登録する
	/// </summary>
	/// <param name="boss"></param>
	void AddListToBoss(Boss* boss);

	/// <summary>
	/// Colliderのポインタからbulletのポインタを探索する
	/// </summary>
	/// <param name="collider">; コライダーのポインタ</param>
	/// <returns></returns>
	BaseEnemy* SearchCollider(AOENGINE::BaseCollider* collider);

private:

	/// <summary>
	/// 敵がカメラに写っているかを判定する関数
	/// </summary>
	/// <param name="enemy">: 敵のポインタ</param>
	/// <returns> true 写っている : false 写っていない</returns>
	bool CheckWithinCamera(BaseEnemy* enemy);

public: // accessor

	const std::list<std::unique_ptr<BaseEnemy>>& GetEnemyList() const { return enemyList_; }

	Boss* GetBoss() const { return pBoss_; }
	BossRoot* GetBossRoot() const { return bossRoot_.get(); }

	void SetPlayer(Player* player) { pPlayer_ = player; }

	// レティクルに一番近いエネミー
	void SetNearReticleEnemy(BaseEnemy* nearEnemy) { nearReticleEnemy = nearEnemy; }
	BaseEnemy* GetNearReticleEnemy() const { return nearReticleEnemy; }

private:

	std::list<std::unique_ptr<BaseEnemy>> enemyList_;

	EnemyFactory enemyFactory_;

	BaseEnemy* nearReticleEnemy;

	Boss* pBoss_;

	Player* pPlayer_;

	std::unique_ptr<BossRoot> bossRoot_;

	std::unique_ptr<EnemyBulletManager> enemyBulletManager_;

};


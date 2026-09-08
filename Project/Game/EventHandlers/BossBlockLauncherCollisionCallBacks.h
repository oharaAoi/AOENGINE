#pragma once

/// stl
#include <string>
#include <unordered_map>
/// engine
#include "Engine/Module/Components/EventHandlers/BaseCollisionCallBacks.h"

/// game
#include "Game/Battle/BlockDamageCalculator.h"

class Boss;
class Block;
class BlockGroupLauncherManager;

/// <summary>
/// Boss と Block の衝突コールバック。
/// Boss が上からブロックに着地した時、そのブロックが属するグループを接続対象へ追加する。
/// </summary>
class BossBlockLauncherCollisionCallBacks : public AOENGINE::BaseCollisionCallBacks{
public:
	BossBlockLauncherCollisionCallBacks() = default;
	~BossBlockLauncherCollisionCallBacks() override = default;

	void Init() override;
	void Update() override;

	void CollisionEnter(AOENGINE::BaseCollider* const bossCollider,AOENGINE::BaseCollider* const blockCollider) override;
	void CollisionStay(AOENGINE::BaseCollider* const bossCollider,AOENGINE::BaseCollider* const blockCollider) override;
	void CollisionExit(AOENGINE::BaseCollider* const bossCollider,AOENGINE::BaseCollider* const blockCollider) override;

private:
	Boss* pBoss_ = nullptr;
	BlockGroupLauncherManager* pLauncherManager_ = nullptr;

	BlockDamageCalculator damageCalculator_;

public: // accessor
	void SetBoss(Boss* boss){ pBoss_ = boss; }
	/// <summary>
	/// ダメージ計算を共有するために渡す。
	/// 集合地点へ出すダメージ表示と、実際に当たった時のダメージを同じ式で求めるため
	/// </summary>
	BlockDamageCalculator* GetDamageCalculator(){ return &damageCalculator_; }
	void SetLauncherManager(BlockGroupLauncherManager* manager){ pLauncherManager_ = manager; }
};

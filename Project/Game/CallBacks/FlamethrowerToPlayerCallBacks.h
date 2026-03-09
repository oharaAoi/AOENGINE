#pragma once
// engine
#include "Engine/Module/Components/EventHandlers/BaseCollisionCallBacks.h"
#include "Engine/System/Manager/CollisionManager.h"
// game
#include "Game/Actor/Boss/Bullet/BossBulletManager.h"
#include "Game/Actor/Player/Player.h"

/// <summary>
/// 火炎放射とプレイヤーの衝突時の処理を行うクラス
/// </summary>
class FlamethrowerToPlayerCallBacks :
	public AOENGINE::BaseCollisionCallBacks {
public:

	FlamethrowerToPlayerCallBacks(AOENGINE::CollisionManager* manager) : pCollisionManager_(manager) {};
	~FlamethrowerToPlayerCallBacks() = default;

public:

	// 初期化
	void Init() override;
	// 更新
	void Update() override;
	// 衝突開始
	void CollisionEnter([[maybe_unused]] AOENGINE::BaseCollider* const bullet, [[maybe_unused]] AOENGINE::BaseCollider* const player) override;
	// 衝突中
	void CollisionStay([[maybe_unused]] AOENGINE::BaseCollider* const bullet, [[maybe_unused]] AOENGINE::BaseCollider* const player)  override;
	// 衝突終わり
	void CollisionExit([[maybe_unused]] AOENGINE::BaseCollider* const bullet, [[maybe_unused]] AOENGINE::BaseCollider* const player)  override;

public:

	void SetBossBulletManager(BossBulletManager* manager) { pBossBulletManager_ = manager; }
	void SetPlayer(Player* player) { pPlayer_ = player; }

private:

	AOENGINE::CollisionManager* pCollisionManager_;

	BossBulletManager* pBossBulletManager_;
	Player* pPlayer_;

};


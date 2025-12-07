#pragma once
#include "Engine/Module/Components/EventHandlers/BaseCollisionCallBacks.h"
#include "Engine/System/Manager/CollisionManager.h"
#include "Engine/Module/Components/Effect/BaseParticles.h"
// Game
#include "Game/Actor/Boss/Bullet/BossBulletManager.h"
#include "Game/Actor/Player/Player.h"

/// <summary>
/// BossのBulletとPlayerのCallBack
/// </summary>
class BBulletToPlayerCallBacks :
	public AOENGINE::BaseCollisionCallBacks {
public:

	BBulletToPlayerCallBacks(AOENGINE::CollisionManager* _collisionManager) : pCollisionManager_(_collisionManager) {};
	~BBulletToPlayerCallBacks() = default;

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

	void SetBossBulletManager(BossBulletManager* _manager) { pBossBulletManager_ = _manager; }

	void SetPlayer(Player* _player) { pPlayer_ = _player; }

private:

	AOENGINE::CollisionManager* pCollisionManager_;

	BossBulletManager* pBossBulletManager_;
	Player* pPlayer_;

	AOENGINE::BaseParticles* hitEffect_;

};


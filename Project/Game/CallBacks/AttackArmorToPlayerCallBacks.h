#pragma once
#include "Engine/Module/Components/EventHandlers/BaseCollisionCallBacks.h"
#include "Engine/System/Manager/CollisionManager.h"
#include "Game/Actor/Player/Player.h"
#include "Game/Actor/Boss/Boss.h"

/// <summary>
/// AttackArmorとPlayerの衝突時の処理
/// </summary>
class AttackArmorToPlayerCallBacks :
	public AOENGINE::BaseCollisionCallBacks {
public:

	AttackArmorToPlayerCallBacks(AOENGINE::CollisionManager* _collisionManager) : pCollisionManager_(_collisionManager) {};
	~AttackArmorToPlayerCallBacks() = default;

public:

	// 初期化
	void Init() override;
	// 更新
	void Update() override;

	// 衝突開始
	void CollisionEnter([[maybe_unused]] AOENGINE::BaseCollider* const armor, [[maybe_unused]] AOENGINE::BaseCollider* const player) override;
	// 衝突中
	void CollisionStay([[maybe_unused]] AOENGINE::BaseCollider* const armor, [[maybe_unused]] AOENGINE::BaseCollider* const player)  override;
	// 衝突終わり
	void CollisionExit([[maybe_unused]] AOENGINE::BaseCollider* const armor, [[maybe_unused]] AOENGINE::BaseCollider* const player)  override;

public:

	void SetPlayer(Player* player) { pPlayer_ = player; }

	void SetBoss(Boss* boss) { pBoss_ = boss; }

private:

	AOENGINE::CollisionManager* pCollisionManager_;

	Player* pPlayer_ = nullptr;
	Boss* pBoss_ = nullptr;
};


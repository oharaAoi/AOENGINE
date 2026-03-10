#pragma once
// engine
#include "Engine/Module/Components/EventHandlers/BaseCollisionCallBacks.h"
#include "Engine/System/Manager/CollisionManager.h"
// game
#include "Game/Actor/Player/Player.h"

class FlamethrowerSwordToPlayerCallBacks :
	public AOENGINE::BaseCollisionCallBacks {
public:

	FlamethrowerSwordToPlayerCallBacks(AOENGINE::CollisionManager* manager) : pCollisionManager_(manager) {};
	~FlamethrowerSwordToPlayerCallBacks() = default;

public:

	// 初期化
	void Init() override;
	// 更新
	void Update() override;
	// 衝突開始
	void CollisionEnter([[maybe_unused]] AOENGINE::BaseCollider* const sword, [[maybe_unused]] AOENGINE::BaseCollider* const player) override;
	// 衝突中
	void CollisionStay([[maybe_unused]] AOENGINE::BaseCollider* const sword, [[maybe_unused]] AOENGINE::BaseCollider* const player)  override;
	// 衝突終わり
	void CollisionExit([[maybe_unused]] AOENGINE::BaseCollider* const sword, [[maybe_unused]] AOENGINE::BaseCollider* const player)  override;

public:

	void SetPlayer(Player* player) { pPlayer_ = player; }

private:

	AOENGINE::CollisionManager* pCollisionManager_;

	Player* pPlayer_;
};


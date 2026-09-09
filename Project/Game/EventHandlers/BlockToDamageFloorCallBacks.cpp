#include "BlockToDamageFloorCallBacks.h"

/// game
#include "Game/Actor/Player/Player.h"
#include "Game/Stage/StageBlockField.h"
#include "Game/WorldObject/Block.h"
#include "Game/WorldObject/StepBlock.h"

void BlockToDamageFloorCallBacks::Init() {
	SetCallBacks();

	// Block と StepBlock は別 category なので、それぞれペアを登録する
	SetPair(pCollisionManager_, kDamageFloorName_, kBlockName_);
	SetPair(pCollisionManager_, kDamageFloorName_, kStepBlockName_);
}

void BlockToDamageFloorCallBacks::CollisionEnter(AOENGINE::BaseCollider* const , AOENGINE::BaseCollider* const block) {
	DestroyOnHit(block);
}

void BlockToDamageFloorCallBacks::CollisionStay(AOENGINE::BaseCollider* const , AOENGINE::BaseCollider* const block) {
	// Enterを取りこぼした時の拾い直し。破壊は1回しか起きないため、Enterと重複しても実害は無い
	DestroyOnHit(block);
}

void BlockToDamageFloorCallBacks::CollisionExit(AOENGINE::BaseCollider* const , AOENGINE::BaseCollider* const ) {
	// 離れた時は何もしない
}

void BlockToDamageFloorCallBacks::DestroyOnHit(AOENGINE::BaseCollider* const blockCollider) {
	if (!pBlockField_) {
		return;
	}

	if (Block* block = pBlockField_->FindBlockByCollider(blockCollider)) {
		const int destroyedGroupId = pBlockField_->DestroyBlockGroup(block);

		if (destroyedGroupId != StageBlockField::kInvalidGroupId && pPlayer_) {
			// 繋いでいたコンボがあれば、受付時間はそのままで0から数え直させる
			pPlayer_->OnConnectedBlockGroupDestroyed(destroyedGroupId);
		}
		return;
	}

	// StepBlock はグループに属さないため、コンボへの通知は不要
	if (StepBlock* stepBlock = pBlockField_->FindStepBlockByCollider(blockCollider)) {
		pBlockField_->DestroyStepBlock(stepBlock);
	}
}

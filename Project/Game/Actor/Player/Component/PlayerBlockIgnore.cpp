#include "PlayerBlockIgnore.h"

#include <algorithm>

#include "Engine/Module/Components/Collider/BaseCollider.h"
#include "Engine/Module/Components/WorldTransform.h"
#include "Engine/System/Manager/CollisionLayerManager.h"

#include "Game/Stage/StageBlockField.h"
#include "Game/WorldObject/Block.h"

using namespace AOENGINE;

///////////////////////////////////////////////////////////////////////////////////////////////
//  大ジャンプの開始
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerBlockIgnore::Begin(const Context& context) {
	isResumed_ = false;
	SetBlockCollisionEnabled(context, false);
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  更新
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerBlockIgnore::Update(const Context& context, bool isAirborne) {

	// 上昇から降下へ移った瞬間に判定を戻す。着地できるようにするため
	const bool startedFalling = context.jumpState == PlayerJump::State::Falling
		&& previousJumpState_ != PlayerJump::State::Falling;

	if (isAirborne && !isResumed_ && startedFalling) {
		ResumeBlockCollision(context);
	}

	previousJumpState_ = context.jumpState;

	// 猶予中のBlockは、離れたものから毎フレーム判定を戻していく
	UpdateIgnoredBlocks(context);
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  着地
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerBlockIgnore::End(const Context& context) {

	// 降下を挟まずに着地した場合でも、判定は必ず戻しておく
	if (!isResumed_) {
		ResumeBlockCollision(context);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  判定の再開
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerBlockIgnore::ResumeBlockCollision(const Context& context) {

	// カテゴリ判定は先に戻しておく
	SetBlockCollisionEnabled(context, true);
	isResumed_ = true;

	// その上で、プレイヤーに埋まっているBlockだけ個別に無効化する
	ignoredBlocks_ = GetOverlappingBlocks(context);
	for (Block* block : ignoredBlocks_) {
		SetBlockActive(block, false);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  猶予中のBlockの見直し
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerBlockIgnore::UpdateIgnoredBlocks(const Context& context) {

	// 判定を無効化しているBlockが無ければ何もしない
	if (ignoredBlocks_.empty()) {
		return;
	}

	// 今フレーム、プレイヤーと実際に重なっているBlockを毎回取り直す
	const std::vector<Block*> currentlyOverlapping = GetOverlappingBlocks(context);

	for (auto it = ignoredBlocks_.begin(); it != ignoredBlocks_.end();) {
		Block* block = *it;

		// 猶予中のBlockが、今もcurrentlyOverlappingに含まれているかを見る
		const bool stillOverlapping =
			std::find(currentlyOverlapping.begin(), currentlyOverlapping.end(), block)
			!= currentlyOverlapping.end();

		if (stillOverlapping) {
			// まだ埋まっているので、判定は無効のまま次のBlockへ
			++it;
			continue;
		}

		// 離れたBlockから判定を元に戻す
		SetBlockActive(block, true);

		// 判定を戻したので猶予リストからも外す
		it = ignoredBlocks_.erase(it);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  重なっているBlockの列挙
///////////////////////////////////////////////////////////////////////////////////////////////

std::vector<Block*> PlayerBlockIgnore::GetOverlappingBlocks(const Context& context) const {

	if (context.blockField == nullptr || context.transform == nullptr) {
		return {};
	}

	// プレイヤーのCollider半サイズぶんのAABBと重なるグリッドマスを調べる
	const Math::Vector3 position = context.transform->GetTranslate();
	return context.blockField->GetBlocksInWorldAABB(
		position - context.overlapHalfExtent, position + context.overlapHalfExtent);
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  当たり判定の切り替え
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerBlockIgnore::SetBlockCollisionEnabled(const Context& context, bool enabled) const {

	if (context.playerCollider == nullptr) {
		return;
	}

	const uint32_t blockBit =
		CollisionLayerManager::GetInstance().GetCategoryBit(kBlockCategoryName);

	if (enabled) {
		context.playerCollider->SetCollisionMaskBit(blockBit);
	} else {
		context.playerCollider->SetCollisionMaskBits(
			context.playerCollider->GetCollisionMaskBit() & ~blockBit);
	}
}

void PlayerBlockIgnore::SetBlockActive(Block* block, bool isActive) const {

	if (block == nullptr) {
		return;
	}

	if (BaseCollider* blockCollider = block->GetCollider(kBlockCategoryName)) {
		blockCollider->SetIsActive(isActive);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  ブロックへの参照を手放す
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerBlockIgnore::ClearBlocks() {
	ignoredBlocks_.clear();
}

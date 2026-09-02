#include "PlayerBlockCollisionCallBacks.h"
#include "Game/Actor/Player/Player.h"
#include "Game/WorldObject/Block.h"

using namespace AOENGINE;

void PlayerBlockCollisionCallBacks::Init() {
	SetCallBacks();
}

void PlayerBlockCollisionCallBacks::Update() {
	// 毎フレームの処理は不要
}

void PlayerBlockCollisionCallBacks::CollisionEnter(AOENGINE::BaseCollider* const playerCollider, AOENGINE::BaseCollider* const blockCollider) {
	TryConnectOnLanding(playerCollider, blockCollider);
}

void PlayerBlockCollisionCallBacks::CollisionStay(AOENGINE::BaseCollider* const, AOENGINE::BaseCollider* const) {
	// 着地は接触が始まった瞬間だけを見るため何もしない
}

void PlayerBlockCollisionCallBacks::CollisionExit(AOENGINE::BaseCollider* const, AOENGINE::BaseCollider* const) {
	// 着地は接触が始まった瞬間だけを見るため何もしない
}

void PlayerBlockCollisionCallBacks::TryConnectOnLanding(AOENGINE::BaseCollider* const playerCollider, AOENGINE::BaseCollider* const blockCollider) {
	// プレイヤーが未設定なら何もできない
	if (!pPlayer_) {
		return;
	}

	// どちらかのColliderが無効なら判定できない
	if (!playerCollider || !blockCollider) {
		return;
	}

	// 落下中でなければ着地ではない
	if (!pPlayer_->IsFalling()) {
		return;
	}

	// 上から乗っていなければ着地ではない
	if (playerCollider->GetCenterPos().y <= blockCollider->GetCenterPos().y) {
		return;
	}

	// 衝突したColliderからBlockを解決できなければ何もしない
	Block* block = FindBlock(blockCollider);
	if (!block) {
		return;
	}

	pPlayer_->TryConnectBlockGroup(block->GetGroupId());
}

bool PlayerBlockCollisionCallBacks::RegisterBlock(Block* block) {
	if (!block) {
		return false;
	}

	AOENGINE::BaseCollider* collider = block->GetCollider(blockColliderTag_);
	if (!collider) {
		// タグ名が合っていない場合ここで落ちる
		return false;
	}

	colliderToBlock_[collider] = block;
	return true;
}

void PlayerBlockCollisionCallBacks::UnregisterBlock(Block* block) {
	// GetCollider() で引き直すと、ブロックのGameObjectが既に破棄されている場合に
	// Colliderが取得できずダングリングしたキーが表に残ってしまうため、
	// 値(block)で走査して一致するエントリを取り除く。
	// 要素数は画面内のブロック数程度で有界なので線形走査で問題ない。
	for (auto it = colliderToBlock_.begin(); it != colliderToBlock_.end(); ) {
		if (it->second == block) {
			it = colliderToBlock_.erase(it);
		} else {
			++it;
		}
	}
}

void PlayerBlockCollisionCallBacks::ClearBlocks() {
	colliderToBlock_.clear();
}

Block* PlayerBlockCollisionCallBacks::FindBlock(const AOENGINE::BaseCollider* collider) const {
	auto it = colliderToBlock_.find(collider);
	if (it == colliderToBlock_.end()) {
		return nullptr;
	}
	return it->second;
}

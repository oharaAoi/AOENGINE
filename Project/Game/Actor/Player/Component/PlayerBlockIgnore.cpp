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

	isDisabled_ = true;
	SetBlockCollisionEnabled(context, false);
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  着地
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerBlockIgnore::OnLanded(const Context& context) {

	// 着地は毎フレーム呼ばれるので、切っている時だけ処理する
	if (!isDisabled_) {
		return;
	}

	// カテゴリ判定は先に戻しておく
	isDisabled_ = false;
	SetBlockCollisionEnabled(context, true);

	// その上で、胴体が埋まっているブロックのグループだけ個別に無効化する
	for (const Block* block : GetBodyOverlappingBlocks(context)) {
		const int groupId = block->GetGroupId();
		if (groupId == StageBlockField::kInvalidGroupId) {
			continue;
		}

		// 同じグループを二重に登録しない
		if (std::find(ignoredGroups_.begin(), ignoredGroups_.end(), groupId) != ignoredGroups_.end()) {
			continue;
		}

		ignoredGroups_.push_back(groupId);
		SetGroupCollisionEnabled(context, groupId, false);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  猶予中のグループの見直し
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerBlockIgnore::Update(const Context& context) {

	if (ignoredGroups_.empty()) {
		return;
	}

	// 今フレーム、胴体が重なっているグループを集め直す
	std::vector<int> overlappingGroups;
	for (const Block* block : GetBodyOverlappingBlocks(context)) {
		const int groupId = block->GetGroupId();
		if (groupId == StageBlockField::kInvalidGroupId) {
			continue;
		}
		if (std::find(overlappingGroups.begin(), overlappingGroups.end(), groupId) == overlappingGroups.end()) {
			overlappingGroups.push_back(groupId);
		}
	}

	for (auto it = ignoredGroups_.begin(); it != ignoredGroups_.end();) {
		const bool stillOverlapping =
			std::find(overlappingGroups.begin(), overlappingGroups.end(), *it) != overlappingGroups.end();

		if (stillOverlapping) {
			// まだ胴体が埋まっているので、判定は無効のまま次のグループへ
			++it;
			continue;
		}

		// 抜けきったグループから判定を元に戻す
		SetGroupCollisionEnabled(context, *it, true);
		it = ignoredGroups_.erase(it);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  胴体と重なっているブロックの列挙
///////////////////////////////////////////////////////////////////////////////////////////////

std::vector<Block*> PlayerBlockIgnore::GetBodyOverlappingBlocks(const Context& context) const {

	if (context.blockField == nullptr || context.transform == nullptr) {
		return {};
	}

	// 胴体の箱と重なるグリッドマスを調べる
	const Math::Vector3 center = context.transform->GetTranslate() + context.bodyOffset;
	const Math::Vector3 half = context.bodySize * 0.5f;

	return context.blockField->GetBlocksInWorldAABB(center - half, center + half);
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  当たり判定の切り替え
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerBlockIgnore::SetBlockCollisionEnabled(const Context& context, bool enabled) const {

	if (context.playerCollider == nullptr) {
		return;
	}

	// Blockのビットだけを触る。Wall(マップチップ2)やダメージ床は残したままにする
	const uint32_t blockBit =
		CollisionLayerManager::GetInstance().GetCategoryBit(kBlockCategoryName);

	if (enabled) {
		context.playerCollider->SetCollisionMaskBit(blockBit);
	} else {
		context.playerCollider->SetCollisionMaskBits(
			context.playerCollider->GetCollisionMaskBit() & ~blockBit);
	}
}

void PlayerBlockIgnore::SetGroupCollisionEnabled(const Context& context, int groupId, bool enabled) const {

	if (context.blockField == nullptr) {
		return;
	}

	const std::vector<Block*>* members = context.blockField->GetGroup(groupId);
	if (members == nullptr) {
		return;
	}

	for (Block* member : *members) {
		if (member == nullptr || !member->IsValid()) {
			continue;
		}

		if (BaseCollider* collider = member->GetCollider(kBlockCategoryName)) {
			collider->SetIsActive(enabled);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  ブロックへの参照を手放す
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerBlockIgnore::ClearGroups() {
	ignoredGroups_.clear();
}

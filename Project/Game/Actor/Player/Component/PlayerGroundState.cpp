#include "PlayerGroundState.h"

#include <algorithm>
#include <cmath>

#include "Engine/Module/Components/Collider/BaseCollider.h"
#include "Engine/Module/Components/Physics/Rigidbody.h"
#include "Engine/Module/Components/WorldTransform.h"

#include "Game/Stage/StageBlockField.h"
#include "Game/WorldObject/Block.h"
#include "Game/WorldObject/Wall.h"

using namespace AOENGINE;

///////////////////////////////////////////////////////////////////////////////////////////////
//  接地判定
///////////////////////////////////////////////////////////////////////////////////////////////

PlayerGroundState::Result PlayerGroundState::Resolve(
	float deltaTime, const Context& context, const Params& params) const {

	Result result{};

	// 押し戻しの向きを見る
	if (context.collider != nullptr) {
		const Math::Vector3& pushback = context.collider->GetPushBackDirection();

		if (pushback.y > kPushbackThreshold) {
			// 下から押し戻された = 足場の上に乗っている
			result.isSupported = true;
		} else if (pushback.y < -kPushbackThreshold) {
			// 上から押し戻された = 頭をぶつけた
			result.hitCeiling = true;
		}
	}

	// 前フレームに進んだぶんも見る範囲に足しておく
	const float fallDistance = std::abs(context.velocityY) * deltaTime;

	// 足元のブロックを直接見る判定と併用して、どちらかで支えられていれば接地とする
	float groundTopY = 0.0f;
	if (TryGetGroundTop(params.groundCheckDistance + fallDistance, context, params, groundTopY)) {
		result.isSupported = true;
	}

	return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  足元の足場の上面を求める
///////////////////////////////////////////////////////////////////////////////////////////////

bool PlayerGroundState::TryGetGroundTop(
	float checkDown, const Context& context, const Params& params, float& outTopY) const {

	if (context.blockField == nullptr || context.transform == nullptr) {
		return false;
	}

	// 大ジャンプ中はブロックとの判定自体を切っているので、足場としても見ない
	if (context.ignoreBlocks) {
		return false;
	}

	// 上がっている最中に拾ってしまうと、飛び出した瞬間に着地扱いになる
	if (context.velocityY > 0.0f) {
		return false;
	}

	// 当たり判定の中心と足元の高さを出す
	const Math::Vector3 center = context.transform->GetTranslate() + params.hitOffset;
	const float feetY = center.y - params.hitSize.y * 0.5f;
	const float halfWidth = params.hitSize.x * 0.5f;

	// 足元の下だけを見る。自分が乗っているマスを拾わないように、少しだけ下から始める
	const Math::Vector3 checkMin{ center.x - halfWidth, feetY - checkDown, center.z };
	const Math::Vector3 checkMax{ center.x + halfWidth, feetY - kGroundCheckEpsilon, center.z };

	bool found = false;
	float topY = 0.0f;

	// 複数マスに跨っている時は、一番高い上面に乗せる
	const auto keepHighest = [&found, &topY](float surfaceY) {
		if (!found || surfaceY > topY) {
			found = true;
			topY = surfaceY;
		}
	};

	for (const Block* block : context.blockField->GetBlocksInWorldAABB(checkMin, checkMax)) {
		if (block == nullptr || !block->IsValid()) {
			continue;
		}

		// 大ジャンプ後に判定を猶予しているブロックは踏めない扱いにする
		if (IsIgnored(context, block)) {
			continue;
		}

		keepHighest(block->GetPosition().y + kBlockHalfHeight);
	}

	// 開幕の床やフィールドの境目は Wall(マップチップ2) で出来ているので、そちらも足場として見る
	for (const Wall* wall : context.blockField->GetWallsInWorldAABB(checkMin, checkMax)) {
		if (wall == nullptr || !wall->IsValid()) {
			continue;
		}

		keepHighest(wall->GetPosition().y + kBlockHalfHeight);
	}

	outTopY = topY;
	return found;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  判定を猶予中のブロックか
///////////////////////////////////////////////////////////////////////////////////////////////

bool PlayerGroundState::IsIgnored(const Context& context, const Block* block) const {
	if (context.ignoredBlocks == nullptr) {
		return false;
	}

	return std::find(context.ignoredBlocks->begin(), context.ignoredBlocks->end(), block)
		!= context.ignoredBlocks->end();
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  足場への吸着
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerGroundState::SnapToGround(const Context& context, const Params& params) const {

	if (!context.isGrounded || context.transform == nullptr) {
		return;
	}

	float groundTopY = 0.0f;
	if (!TryGetGroundTop(params.groundCheckDistance, context, params, groundTopY)) {
		return;
	}

	// 接地中は足場へ押し付ける速度がかかり続けるので、押し戻し任せだと
	// 毎フレーム沈んでは戻される形になって上下に震える。
	// 下にある足場の上面は分かっているので、その高さへ直接置き直す
	Math::Vector3 position = context.transform->GetTranslate();
	position.y = groundTopY + params.hitSize.y * 0.5f - params.hitOffset.y;
	context.transform->SetTranslate(position);

	// 沈み込む速度を残さない
	if (context.rigidbody != nullptr) {
		context.rigidbody->SetVelocityY(0.0f);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  落下の下限
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerGroundState::ClampFallLimit(const Context& context, const Params& params) const {

	if (context.transform == nullptr) {
		return;
	}

	Math::Vector3 position = context.transform->GetTranslate();
	if (position.y > params.fallLimitY) {
		return;
	}

	// 下限より下がったら、その高さに置き直して止める
	position.y = params.fallLimitY;
	context.transform->SetTranslate(position);

	// 速度を残すと毎フレーム下限へ引き戻す形になるので、落下を止めておく
	if (context.rigidbody != nullptr) {
		context.rigidbody->SetVelocityY(0.0f);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  奥行きの固定
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerGroundState::FixZPosition(const Context& context, const Params& params) const {

	if (context.transform == nullptr) {
		return;
	}

	// ブロックとの押し戻しは一番浅い軸へ返るため、条件次第でZ方向にも押される
	Math::Vector3 position = context.transform->GetTranslate();
	position.z = params.fixedZ;
	context.transform->SetTranslate(position);

	// 速度が残っていると次のフレームでまたずれるので、こちらも止めておく
	if (context.rigidbody != nullptr) {
		context.rigidbody->SetVelocityZ(0.0f);
	}
}

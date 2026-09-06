#include "PlayerGroundState.h"

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

	// 落下が速いと1フレームでブロックを跨いでしまうので、
	// 前フレームに進んだぶんも見る範囲に足しておく
	const float checkDown = params.groundCheckDistance + std::abs(context.velocityY) * deltaTime;

	// 押し戻しの向きを見る
	if (context.collider != nullptr) {
		const Math::Vector3& pushback = context.collider->GetPushBackDirection();

		if (pushback.y < -kPushbackThreshold) {
			// 上から押し戻された = 頭をぶつけた
			result.hitCeiling = true;
		} else if (pushback.y > kPushbackThreshold && !context.isBigJump) {
			// 下から押し戻された = 足場の上に乗っている。
			// 大ジャンプ中はBlockとの判定を切っているので、押し戻しでは着地を決めない
			result.isSupported = true;
		}
	}

	// 押し戻しは一番浅い軸へ1本しか返らないため、横のブロックに触れたフレームで
	// Y成分が消えて接地が外れ、歩いている最中に落下扱いになってしまう。
	// 足元を直接見る判定と併用して、どちらかで支えられていれば接地とする
	float groundTopY = 0.0f;
	if (TryGetGroundTop(checkDown, context, params, groundTopY)) {
		result.isSupported = true;
		result.hasGroundTop = true;
		result.groundTopY = groundTopY;
	}

	return result;
}


///////////////////////////////////////////////////////////////////////////////////////////////
//  箱の中心
///////////////////////////////////////////////////////////////////////////////////////////////

Math::Vector3 PlayerGroundState::CalcBoxCenter(const Context& context, const Math::Vector3& offset) const {
	return context.transform->GetTranslate() + offset;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  足元の足場の上面を求める
///////////////////////////////////////////////////////////////////////////////////////////////

bool PlayerGroundState::TryGetGroundTop(
	float checkDown, const Context& context, const Params& params, float& outTopY) const {

	if (context.blockField == nullptr || context.transform == nullptr) {
		return false;
	}

	// 上がっている最中に拾ってしまうと、飛び出した瞬間に着地扱いになる
	if (context.velocityY > 0.0f) {
		return false;
	}

	// 足元の箱の底から下だけを見る。自分が乗っているマスを拾わないよう少しだけ下から始める
	const Math::Vector3 footCenter = CalcBoxCenter(context, params.footOffset);
	const float feetY = footCenter.y - params.footSize.y * 0.5f;
	const float halfWidth = params.footSize.x * 0.5f;

	const Math::Vector3 checkMin{ footCenter.x - halfWidth, feetY - checkDown, footCenter.z };
	const Math::Vector3 checkMax{ footCenter.x + halfWidth, feetY - kGroundCheckEpsilon, footCenter.z };

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
		keepHighest(block->GetPosition().y + kBlockHalfHeight);
	}

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
//  足場の上面から、本体を置くY座標を求める
///////////////////////////////////////////////////////////////////////////////////////////////

float PlayerGroundState::CalcStandY(float groundTopY, const Params& params) const {
	// 足元の箱の底が、足場の上面にぴったり乗る高さ
	return groundTopY + params.footSize.y * 0.5f - params.footOffset.y;
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
	position.y = CalcStandY(groundTopY, params);
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
//  壁で左右を止める
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerGroundState::ClampToWalls(const Context& context, const Params& params) const {

	if (context.transform == nullptr || context.blockField == nullptr) {
		return;
	}


	// 壁の上に立っているだけのマスを横の壁と取り違えない
	const Math::Vector3 center = CalcBoxCenter(context, params.bodyOffset);
	const Math::Vector3 half = params.bodySize * 0.5f;

	const std::vector<Wall*> walls =
		context.blockField->GetWallsInWorldAABB(center - half, center + half);
	if (walls.empty()) {
		return;
	}

	// 胴体が入り込まない位置へ直接置き直す
	bool hasRightLimit = false;
	bool hasLeftLimit = false;
	float rightLimit = 0.0f;	// 胴体の中心がこれより右へは行けない
	float leftLimit = 0.0f;		// 胴体の中心がこれより左へは行けない

	for (const Wall* wall : walls) {
		if (wall == nullptr || !wall->IsValid()) {
			continue;
		}

		const float wallX = wall->GetPosition().x;

		if (wallX > center.x) {
			// 右にある壁。その左面より内側で止める
			const float limit = wallX - kBlockHalfWidth - half.x;
			if (!hasRightLimit || limit < rightLimit) {
				hasRightLimit = true;
				rightLimit = limit;
			}
		} else {
			// 左にある壁。その右面より内側で止める
			const float limit = wallX + kBlockHalfWidth + half.x;
			if (!hasLeftLimit || limit > leftLimit) {
				hasLeftLimit = true;
				leftLimit = limit;
			}
		}
	}

	float clampedX = center.x;
	if (hasRightLimit && clampedX > rightLimit) {
		clampedX = rightLimit;
	}
	if (hasLeftLimit && clampedX < leftLimit) {
		clampedX = leftLimit;
	}

	if (clampedX == center.x) {
		return;
	}

	// 箱の中心をずらしたぶん、本体も同じだけ動かす
	Math::Vector3 position = context.transform->GetTranslate();
	position.x += clampedX - center.x;
	context.transform->SetTranslate(position);

	// 速度を残すと壁へ押し付け続ける形になるので止めておく
	if (context.rigidbody != nullptr) {
		context.rigidbody->SetVelocityX(0.0f);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  奥行きの固定
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerGroundState::FixZPosition(const Context& context, const Params& params) const {

	if (context.transform == nullptr) {
		return;
	}

	// ブロックとの押し戻しは一番浅い軸へ返るため、条件次第でZ方向にも押される。
	// 横スクロールなので奥行きは動かさず、毎フレーム決まった位置へ戻す
	Math::Vector3 position = context.transform->GetTranslate();
	position.z = params.fixedZ;
	context.transform->SetTranslate(position);

	// 速度が残っていると次のフレームでまたずれるので、こちらも止めておく
	if (context.rigidbody != nullptr) {
		context.rigidbody->SetVelocityZ(0.0f);
	}
}

#include "PlayerBlockCollisionCallBacks.h"
#include "Game/Actor/Player/Player.h"
#include "Game/Stage/StageBlockField.h"
#include "Game/WorldObject/Block.h"

/// math
#include "Engine/Lib/Math/MyMath.h"

/// engine
#include "Engine/Lib/GameTimer.h"

/// stl
#include <algorithm>
#include <cmath>

using namespace AOENGINE;

namespace {

	/// <summary>
	/// 着地とみなすために必要な、ブロックの上面との水平方向の重なり量。
	/// 角にかすっただけの接触や、側面にめり込んだだけの接触を足場として拾わないための下限。
	/// 横移動の1フレーム分のめり込み量(moveSpeed * deltaTime)より大きくしておくこと
	/// </summary>
	constexpr float kMinLandingOverlap = 0.2f;

	/// <summary>
	/// 上面を跨いだかを見る時の余裕。押し戻しや吸着で多少ずれても着地を拾えるようにする
	/// </summary>
	constexpr float kLandingPenetrationMargin = 0.1f;

	/// <summary>
	/// 着地したブロックの上に、プレイヤーが入るために必要な空きマス数
	/// </summary>
	constexpr int kRequiredSpaceCellCount = 1;

}

void PlayerBlockCollisionCallBacks::Init() {
	SetCallBacks();
}

void PlayerBlockCollisionCallBacks::Update() {
	// 毎フレームの処理は不要
}

void PlayerBlockCollisionCallBacks::CollisionEnter(AOENGINE::BaseCollider* const playerCollider, AOENGINE::BaseCollider* const blockCollider) {
	TryConnectOnLanding(playerCollider, blockCollider);
}

void PlayerBlockCollisionCallBacks::CollisionStay(AOENGINE::BaseCollider* const playerCollider, AOENGINE::BaseCollider* const blockCollider) {
	// 着地したフレームと接触が始まるフレームは必ずしも一致しない。
	// 判定は Enter と全く同じ（着地直後で、上面を跨いだ接触の時だけ通る）なので、
	// 取りこぼしたぶんを拾い直せるように接触中も見ておく
	TryConnectOnLanding(playerCollider, blockCollider);
}

void PlayerBlockCollisionCallBacks::CollisionExit(AOENGINE::BaseCollider* const, AOENGINE::BaseCollider* const) {
	// 着地は接触している間だけを見るため、離れた時は何もしない
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

	// 接続の条件は「ジャンプして着地したか」。
	// 落下中というだけでは、横のブロックに触れながら落ちている状態も通ってしまうため、
	// 空中から足場へ降りた直後(Player側で立てる受付)だけを見る
	if (!pPlayer_->IsJustLanded()) {
		return;
	}

	// 上へ動いている間の接触は、下から潜り込んだ当たりなので着地ではない
	if (pPlayer_->GetVelocity().y > 0.0f) {
		return;
	}

	// 衝突したColliderからBlockを解決できなければ何もしない
	Block* block = FindBlock(blockCollider);
	if (!block) {
		return;
	}

	// 上から乗った接触か（横・下から当たっただけなら弾く）
	if (!IsLandingContact(playerCollider, blockCollider)) {
		return;
	}

	// 実際に乗れる足場か（真上が埋まっているブロックの側面をなぞっただけなら弾く）
	if (!IsLandablePlatform(block)) {
		return;
	}

	pPlayer_->TryConnectBlockGroup(block->GetGroupId());
}

bool PlayerBlockCollisionCallBacks::IsLandingContact(const AOENGINE::BaseCollider* playerCollider, const AOENGINE::BaseCollider* blockCollider) const {

	// AABB以外の形状は想定していない。判断できないものは着地とみなさない
	const auto* playerShape = std::get_if<Math::AABB>(&playerCollider->GetShape());
	const auto* blockShape = std::get_if<Math::AABB>(&blockCollider->GetShape());
	if (!playerShape || !blockShape) {
		return false;
	}

	// 各軸の重なり量を出す。3軸すべてが正の時だけ実際に重なっている
	const Math::Vector3 overlap{
		(std::min)(playerShape->max.x, blockShape->max.x) - (std::max)(playerShape->min.x, blockShape->min.x),
		(std::min)(playerShape->max.y, blockShape->max.y) - (std::max)(playerShape->min.y, blockShape->min.y),
		(std::min)(playerShape->max.z, blockShape->max.z) - (std::max)(playerShape->min.z, blockShape->min.z)
	};

	if (overlap.x <= 0.0f || overlap.y <= 0.0f || overlap.z <= 0.0f) {
		return false;
	}

	// 上面にきちんと重なっているか。
	// 側面をなぞっているだけの接触は横方向の重なりが1フレームのめり込み量しか無いため、
	// ここで縦に積まれたブロックへの横当たりが落ちる
	if (overlap.x < kMinLandingOverlap || overlap.z < kMinLandingOverlap) {
		return false;
	}

	// 侵入軸がYか(=このフレームに上面を跨いだか)を、潜り込んだ深さで確かめる。
	// 深さの比較(一番浅い軸を侵入軸とみなす方法)は、落下が速いフレームだと
	// Y方向の重なりが横幅を超えてしまい正しい着地まで落としてしまうため、
	// 「1フレームで落ちられる距離」を上限にして判断する
	const float fallStep = std::abs(pPlayer_->GetVelocity().y) * AOENGINE::GameTimer::DeltaTime();
	const float allowedPenetration = fallStep + kLandingPenetrationMargin;

	// ブロックの上面から足元がどれだけ下に入り込んでいるか。
	// 横から当たった時や下から潜り込んだ時は、この値が1フレームの落下距離を大きく超える
	const float penetration = blockShape->max.y - playerShape->min.y;
	if (penetration > allowedPenetration) {
		return false;
	}

	return true;
}

bool PlayerBlockCollisionCallBacks::IsLandablePlatform(const Block* block) const {

	// 表が未設定の時は、グリッドでの確認を省いて接触だけで判断する
	if (!pBlockField_) {
		return true;
	}

	const GridPos& gridPos = block->GetGridPos();

	// 段から切り離されたブロック（集合・打ち上げ中のもの）はグリッド上に居ないため、足場にはしない。
	// GetGridPos() は切り離す前の値が残っているので、今もそのマスの持ち主かどうかで確かめる
	if (pBlockField_->GetBlockAt(gridPos) != block) {
		return false;
	}

	// 真上が埋まっているブロックには、そもそもプレイヤーが乗るスペースが無い
	return pBlockField_->HasSpaceAbove(gridPos, kRequiredSpaceCellCount);
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

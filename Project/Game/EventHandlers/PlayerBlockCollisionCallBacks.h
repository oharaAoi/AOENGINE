#pragma once
/// stl
#include <string>
#include <unordered_map>
/// engine
#include "Engine/Module/Components/EventHandlers/BaseCollisionCallBacks.h"

class Player;
class Block;
class StageBlockField;

/// <summary>
/// Player と Block の衝突コールバック。
/// プレイヤーが上からブロックに着地した時、そのブロックが属するグループを接続対象へ追加する。
/// 着地した足場は Player 側が接地判定から直接引いて接続するため、こちらは
/// 「押し戻しだけで支えられて足場を特定できなかった」場合を拾う補助の経路になる。
/// </summary>
class PlayerBlockCollisionCallBacks : public AOENGINE::BaseCollisionCallBacks {
public:
	PlayerBlockCollisionCallBacks() = default;
	~PlayerBlockCollisionCallBacks() override = default;

	void Init() override;
	void Update() override;

	void CollisionEnter(AOENGINE::BaseCollider* const playerCollider, AOENGINE::BaseCollider* const blockCollider) override;
	void CollisionStay(AOENGINE::BaseCollider* const playerCollider, AOENGINE::BaseCollider* const blockCollider) override;
	void CollisionExit(AOENGINE::BaseCollider* const playerCollider, AOENGINE::BaseCollider* const blockCollider) override;

	/// <summary>ブロックを Collider -> Block の解決表に登録する</summary>
	bool RegisterBlock(Block* block);
	/// <summary>解決表からブロックを取り除く</summary>
	void UnregisterBlock(Block* block);
	/// <summary>解決表を空にする</summary>
	void ClearBlocks();

private:
	/// <summary>着地とみなせる衝突なら、そのブロックのグループを接続対象へ追加する</summary>
	void TryConnectOnLanding(AOENGINE::BaseCollider* const playerCollider, AOENGINE::BaseCollider* const blockCollider);

	/// <summary>
	/// 上から乗った接触かどうかを、2つのAABBの重なり方から判定する。
	/// ブロックの上面に水平方向で十分重なっていて、なおかつ足元が
	/// 「1フレームで落ちられる距離」の範囲でしか上面より下に入っていない
	/// （＝そのフレームに上面を跨いだ＝侵入軸がY）時だけ着地とみなす。
	/// </summary>
	bool IsLandingContact(const AOENGINE::BaseCollider* playerCollider, const AOENGINE::BaseCollider* blockCollider) const;

	/// <summary>
	/// そのブロックが実際に乗れる足場かどうかを、グリッド上の状況から判定する。
	/// 真上が埋まっている（＝プレイヤーが入れない）ブロックは足場として扱わない。
	/// </summary>
	bool IsLandablePlatform(const Block* block) const;

	/// <summary>Collider から Block を引く（未登録なら nullptr）</summary>
	Block* FindBlock(const AOENGINE::BaseCollider* collider) const;

	Player* pPlayer_ = nullptr;                 // 非所有
	const StageBlockField* pBlockField_ = nullptr;  // 非所有。未設定ならグリッドでの確認は行わない
	std::string blockColliderTag_ = "Block";    // ブロック側の Collider category 名
	std::unordered_map<const AOENGINE::BaseCollider*, Block*> colliderToBlock_;  // 非所有

public: // accessor
	void SetPlayer(Player* player) { pPlayer_ = player; }
	/// <summary>着地判定でグリッドを引くための連結グループ表を設定する（非所有）</summary>
	void SetBlockField(const StageBlockField* field) { pBlockField_ = field; }
	void SetBlockColliderTag(const std::string& tag) { blockColliderTag_ = tag; }
};

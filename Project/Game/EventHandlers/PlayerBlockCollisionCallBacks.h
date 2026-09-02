#pragma once
/// stl
#include <string>
#include <unordered_map>
/// engine
#include "Engine/Module/Components/EventHandlers/BaseCollisionCallBacks.h"

class Player;
class Block;

/// <summary>
/// Player と Block の衝突コールバック。
/// プレイヤーが上からブロックに着地した時、そのブロックが属するグループを接続対象へ追加する。
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
	/// <summary>Collider から Block を引く（未登録なら nullptr）</summary>
	Block* FindBlock(const AOENGINE::BaseCollider* collider) const;

	Player* pPlayer_ = nullptr;                 // 非所有
	std::string blockColliderTag_ = "Block";    // ブロック側の Collider category 名
	std::unordered_map<const AOENGINE::BaseCollider*, Block*> colliderToBlock_;  // 非所有

public: // accessor
	void SetPlayer(Player* player) { pPlayer_ = player; }
	void SetBlockColliderTag(const std::string& tag) { blockColliderTag_ = tag; }
};

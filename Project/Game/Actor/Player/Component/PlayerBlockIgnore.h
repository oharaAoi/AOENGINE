#pragma once
#include <string>
#include <vector>

#include "Engine/Lib/Math/Vector3.h"
#include "Game/Actor/Player/Component/PlayerJump.h"

namespace AOENGINE {
	class BaseCollider;
	class WorldTransform;
}

class Block;
class StageBlockField;

/// <summary>
/// ダメージ床の大ジャンプ中だけ、Blockとの当たり判定を外すコンポーネント
/// </summary>
class PlayerBlockIgnore {
public:

	// 判断に使う今の状況
	struct Context {
		AOENGINE::BaseCollider* playerCollider = nullptr;	// プレイヤーのCollider
		const AOENGINE::WorldTransform* transform = nullptr;// プレイヤーのTransform
		const StageBlockField* blockField = nullptr;		// ブロックを引く表
		Math::Vector3 overlapHalfExtent{ 0.5f, 0.5f, 0.5f };// 埋まり判定に使う半サイズ
		PlayerJump::State jumpState = PlayerJump::State::Grounded;
	};

	PlayerBlockIgnore() = default;
	~PlayerBlockIgnore() = default;

	/// <summary>大ジャンプの開始。Blockとの判定を丸ごと外す</summary>
	void Begin(const Context& context);

	/// <summary>
	/// 降下に移ったら判定を戻し、猶予中のBlockは離れたものから戻していく
	/// </summary>
	void Update(const Context& context, bool isAirborne);

	/// <summary>着地。猶予を全て解除して元に戻す</summary>
	void End(const Context& context);

	/// <summary>ステージが作り直される時に、ブロックへの参照を全て手放す</summary>
	void ClearBlocks();

private:

	/// <summary>Blockカテゴリとの当たり判定を切り替える</summary>
	void SetBlockCollisionEnabled(const Context& context, bool enabled) const;

	/// <summary>プレイヤーと今重なっているBlockを列挙する</summary>
	std::vector<Block*> GetOverlappingBlocks(const Context& context) const;

	/// <summary>判定を戻し、その時に埋まっているBlockだけ個別に無効化する</summary>
	void ResumeBlockCollision(const Context& context);

	/// <summary>猶予中のBlockを見直し、離れたものから判定を戻す</summary>
	void UpdateIgnoredBlocks(const Context& context);

	/// <summary>1個のBlockの当たり判定を切り替える</summary>
	void SetBlockActive(Block* block, bool isActive) const;

private:

	// 判定を猶予しているBlock
	std::vector<Block*> ignoredBlocks_;

	// 今回の大ジャンプで、降下時の判定再開をもう済ませたか
	bool isResumed_ = true;

	// 直前フレームのジャンプ状態。降下へ移った瞬間を拾うために持つ
	PlayerJump::State previousJumpState_ = PlayerJump::State::Grounded;

	// Collider category名
	static inline const std::string kBlockCategoryName = "Block";

public: // accessor

	/// <summary>今Blockとの判定を丸ごと切っている最中か</summary>
	bool IsBlockCollisionDisabled() const { return !isResumed_; }

	/// <summary>判定を猶予中のBlock。接地判定から踏めない扱いにするために渡す</summary>
	const std::vector<Block*>& GetIgnoredBlocks() const { return ignoredBlocks_; }
};

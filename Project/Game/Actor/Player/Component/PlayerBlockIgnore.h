#pragma once
#include <string>
#include <vector>

#include "Engine/Lib/Math/Vector3.h"

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
		const StageBlockField* blockField = nullptr;			// ブロックを引く表
		Math::Vector3 bodySize{ 1.0f, 1.0f, 1.0f };			// 胴体から頭までの箱の大きさ
		Math::Vector3 bodyOffset{ 0.0f, 0.0f, 0.0f };		// その箱の中心のずらし量
	};

	PlayerBlockIgnore() = default;
	~PlayerBlockIgnore() = default;

	/// <summary>大ジャンプの開始。Blockとの当たり判定だけを外す</summary>
	void Begin(const Context& context);

	/// <summary>
	/// 着地
	/// </summary>
	void OnLanded(const Context& context);

	/// <summary>猶予中のグループを毎フレーム見直し、胴体が抜けたものから判定を戻す</summary>
	void Update(const Context& context);

	/// <summary>ステージが作り直される時に、ブロックへの参照を全て手放す</summary>
	void ClearGroups();

private:

	/// <summary>Blockカテゴリとの当たり判定を切り替える</summary>
	void SetBlockCollisionEnabled(const Context& context, bool enabled) const;

	/// <summary>胴体の箱と今重なっているブロックを列挙する</summary>
	std::vector<Block*> GetBodyOverlappingBlocks(const Context& context) const;

	/// <summary>グループに属する全ブロックの当たり判定を切り替える</summary>
	void SetGroupCollisionEnabled(const Context& context, int groupId, bool enabled) const;

private:

	// 今、Blockとの判定を切っている最中か
	bool isDisabled_ = false;

	// 判定を猶予している連結グループのID
	std::vector<int> ignoredGroups_;

	// 外す対象のCollider category名
	static inline const std::string kBlockCategoryName = "Block";

public: // accessor

	/// <summary>Blockとの判定を切っている最中か</summary>
	bool IsBlockCollisionDisabled() const { return isDisabled_; }

	/// <summary>判定を猶予中のグループがあるか</summary>
	bool HasIgnoredGroup() const { return !ignoredGroups_.empty(); }
};

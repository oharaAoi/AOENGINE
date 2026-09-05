#pragma once
#include <vector>

#include "Engine/Lib/Math/Vector3.h"

namespace AOENGINE {
	class BaseCollider;
	class Rigidbody;
	class WorldTransform;
}

class Block;
class StageBlockField;

/// <summary>
/// プレイヤーの接地判定と、位置の補正を行うコンポーネント。
/// </summary>
class PlayerGroundState {
public:

	// 調整値
	struct Params {
		Math::Vector3 footSize;		// 足元の箱の大きさ
		Math::Vector3 footOffset;	// 足元の箱の中心のずらし量
		float groundCheckDistance;	// 足元の何ユニット下までを足場として見るか
		float fallLimitY;			// これ以上は落ちない高さ
		float fixedZ;				// 固定する奥行き
	};

	// 判断に使う今の状況
	struct Context {
		AOENGINE::WorldTransform* transform = nullptr;	// 本体のTransform
		AOENGINE::Rigidbody* rigidbody = nullptr;		// 本体のRigidbody
		const AOENGINE::BaseCollider* collider = nullptr;// 押し戻しを見るCollider
		const StageBlockField* blockField = nullptr;		// 足場を引く表
		float velocityY = 0.0f;		// 今の縦速度
		bool isGrounded = false;	// 接地状態か
		bool isBigJump = false;		// ダメージ床の大ジャンプ中か
	};

	// 接地判定の結果。状態の遷移そのものは呼び出し側に任せる
	struct Result {
		bool isSupported = false;	// 足場に支えられているか
		bool hitCeiling = false;	// 上から押し戻されたか
		bool hasGroundTop = false;	// 乗る足場の上面が分かっているか
		float groundTopY = 0.0f;	// その上面の高さ
	};

	PlayerGroundState() = default;
	~PlayerGroundState() = default;

	/// <summary>足場に乗っているかを判定する</summary>
	Result Resolve(float deltaTime, const Context& context, const Params& params) const;

	/// <summary>接地中は、足元のブロックの上面へ直接置き直す</summary>
	void SnapToGround(const Context& context, const Params& params) const;

	/// <summary>足場の上面の高さから、本体を置くべきY座標を求める</summary>
	float CalcStandY(float groundTopY, const Params& params) const;

	/// <summary>落下の下限。これより下がったらその高さで止める</summary>
	void ClampFallLimit(const Context& context, const Params& params) const;

	/// <summary>押し戻しで奥行きがずれても、決まったZへ固定し直す</summary>
	void FixZPosition(const Context& context, const Params& params) const;

private:

	/// <summary>
	/// 足元の箱の下を見て、そこにあるブロックの上面の高さを求める。
	/// </summary>
	bool TryGetGroundTop(float checkDown, const Context& context,
		const Params& params, float& outTopY) const;

	/// <summary>箱の中心のワールド座標を求める</summary>
	Math::Vector3 CalcBoxCenter(const Context& context, const Math::Vector3& offset) const;

private:

	// 押し戻しを接地・天井とみなす閾値
	static constexpr float kPushbackThreshold = 0.0001f;
	// 足元判定で、自分が乗っているマスを拾わないように空ける隙間
	static constexpr float kGroundCheckEpsilon = 0.001f;
	// ブロック1個の高さの半分。上面の高さを出すのに使う
	static constexpr float kBlockHalfHeight = 0.5f;
};

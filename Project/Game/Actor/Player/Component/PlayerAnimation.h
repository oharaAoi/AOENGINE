#pragma once
#include <string>

namespace AOENGINE {
	class Animator;
}

/// <summary>
/// プレイヤーの状態に合わせてアニメーションを切り替えるコンポーネント
/// </summary>
class PlayerAnimation {
public:

	// 調整値
	struct Params {
		float blendSpeed;			// 切り替える時の補間速度
		float defaultSpeed;			// 既定の再生速度
		float walkSpeed;			// 歩きの再生速度
		float idleDelay;			// 入力が途切れてから直立へ戻すまでの猶予
		float moveInputThreshold;	// 移動しているとみなす入力・速度の下限
	};

	// 判断に使う今の状況
	struct Context {
		AOENGINE::Animator* animator = nullptr;	// 切り替える対象
		float horizontal = 0.0f;				// 左右入力
		float velocityX = 0.0f;					// 実際の横速度
		bool isGrounded = true;					// 接地しているか
	};

	PlayerAnimation() = default;
	~PlayerAnimation() = default;

	// 初期化、更新
	void Init();
	void Update(float deltaTime, const Context& context, const Params& params);

private:

	/// <summary>今の状況から流したいアニメーションの名前を決める</summary>
	const std::string& SelectName(const Context& context, const Params& params) const;

	/// <summary>アニメーションごとの再生速度を決める</summary>
	float SelectSpeed(const std::string& name, const Params& params) const;

	/// <summary>移動中とみなせるかを、入力と実際の速度の両方から見る</summary>
	bool IsMoving(const Context& context, const Params& params) const;

private:

	// 左右の移動入力・速度が無い状態が続いている時間
	float noMoveInputTimer_ = 0.0f;

	// 直近で流そうとしたアニメーション名
	std::string currentName_;

	// モデルに入っているアニメーション名
	static inline const std::string kIdleName = "idle";
	static inline const std::string kWalkName = "walk";
	static inline const std::string kJumpName = "jump";

public: // accessor

	const std::string& GetCurrentName() const { return currentName_; }
};

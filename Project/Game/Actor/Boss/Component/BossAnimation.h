#pragma once
#include <string>

namespace AOENGINE {
	class Animator;
}

/// <summary>
/// ボスの行動に合わせてアニメーションを切り替えるコンポーネント。
/// 被弾だけは行動に割り込んで1回だけ流し、終わったら行動側のものへ戻る。
/// </summary>
class BossAnimation {
public:

	// 調整値
	struct Params {
		float blendSpeed;	// 切り替える時の補間速度
		float speed;		// 再生速度
	};

	// 判断に使う今の状況
	struct Context {
		AOENGINE::Animator* animator = nullptr;	// 切り替える対象(非所有)
		const std::string* behaviorName = nullptr;// 今の行動が指定するアニメーション名(非所有)
	};

	BossAnimation() = default;
	~BossAnimation() = default;

	// 初期化、更新
	void Init();
	void Update(const Context& context, const Params& params);

	/// <summary>被弾を1回だけ割り込ませる</summary>
	void PlayDamage();

private:

	/// <summary>今流すべきアニメーション名を決める</summary>
	const std::string& SelectName(const Context& context) const;

	/// <summary>被弾が流れ終わったかを見て、終わっていたら割り込みを解除する</summary>
	void UpdateDamageState(const Context& context);

private:

	// 被弾を流している最中か
	bool isDamagePlaying_ = false;

	// モデルに入っている被弾アニメーションの名前
	static inline const std::string kDamageName = "damage";
	// 行動側から名前が来なかった時に流すもの
	static inline const std::string kDefaultName = "idle";

public: // accessor

	bool IsDamagePlaying() const { return isDamagePlaying_; }
};

#pragma once
#include <string>

namespace AOENGINE {
	class Animator;
}

/// <summary>
/// ボスの行動に合わせてアニメーションを切り替えるコンポーネント。
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

	/// <summary>同じアニメーションでも頭から流し直す</summary>
	void Replay();

private:

	/// <summary>今の行動が指定するアニメーション名。無ければ待機</summary>
	const std::string& SelectBehaviorName(const Context& context) const;

	/// <summary>今流すべきアニメーション名を決める</summary>
	const std::string& SelectName(const Context& context) const;

	/// <summary>指定した名前のクリップが今流れていて、最後まで行ったか</summary>
	bool IsClipFinished(const Context& context, const std::string& name) const;

	/// <summary>被弾が流れ終わったかを見て、終わっていたら割り込みを解除する</summary>
	void UpdateDamageState(const Context& context);

	/// <summary>行動のアニメーションが流れ切ったかを見る</summary>
	void UpdateBehaviorFinishState(const Context& context, const std::string& behaviorName);

private:

	// 被弾を流している最中か
	bool isDamagePlaying_ = false;

	// 同じクリップを頭から流し直す要求が来ているか
	bool isReplayRequested_ = false;

	// 行動のアニメーションが最後まで流れ切ったか
	bool isBehaviorFinished_ = false;

	// 直前に見た行動のアニメーション名。行動が変わった時に数え直すために持つ
	std::string lastBehaviorName_;

	// モデルに入っている被弾アニメーションの名前
	static inline const std::string kDamageName = "damage";
	// 待機。行動側から名前が来なかった時にも流す
	static inline const std::string kIdleName = "idle";

public: // accessor

	bool IsDamagePlaying() const { return isDamagePlaying_; }
};

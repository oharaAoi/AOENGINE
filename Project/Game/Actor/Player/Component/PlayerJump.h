#pragma once
#include <array>
#include <cstdint>
#include<string>
#include <functional>

/// <summary>
/// プレイヤーのジャンプ処理
/// </summary>
class PlayerJump {
public:

	// ジャンプの状態
	enum class State {
		Grounded,	// 接地
		Rising,		// 上昇中
		Hanging,	// 頂点で滞空中
		Falling,	// 落下中
	};

	// 調整値
	struct Params {
		float jumpPower;
		// 空中ジャンプの強さ。地上のジャンプとは別に持つ
		float airJumpPower;
		// 接地から着地までに跳べる回数。1で空中ジャンプ無し、2で2段ジャンプ
		int32_t maxJumpCount;
		// 上昇中に入力を離した時、残す上昇速度の割合。小さいほど小ジャンプになる
		float releaseRiseRate;
		float hangTime;
		// ダメージ床で飛ばされた時の滞空時間。自分のジャンプとは別に持つ
		float knockbackHangTime;
		float riseGravity;
		float fallGravity;
		float maxFallSpeed;
		float groundKeepSpeed;	// 接地中に足場へ押し付ける速度
	};

	PlayerJump();
	~PlayerJump() = default;

	// 更新、着地
	void Update(float deltaTime, bool jumpTriggered, bool jumpHeld, const Params& params);
	void Land();
	// 足場から外れた
	void LeaveGround();
	// 上昇中に頭をぶつけた
	void HitCeiling();
	// ダメージ床によるノックバックを開始する
	void Knockback(float power);

private:
	std::size_t ToIndex(State state) const { return static_cast<std::size_t>(state); }
	void ChangeState(State next);

	/// <summary>
	/// 空中ジャンプを試す。跳べたらtrueを返す
	/// </summary>
	bool TryAirJump(bool jumpTriggered);
private:

	// 状態の数
	static constexpr std::size_t kStateCount = 4;

	State state_ = State::Grounded;
	float velocityY_ = 0.0f;
	float hangTimer_ = 0.0f;
	bool jumpStarted_ = false;
	// 接地してから跳んだ回数。maxJumpCountに達したら空中では跳べない
	int32_t jumpCount_ = 0;
	// ジャンプ入力が押しっぱなしか。滞空を打ち切るかの判断に使う
	bool isJumpHeld_ = false;
	// 今のジャンプが自分のジャンプ入力から始まったものか。
	bool isPlayerJump_ = false;

	// パラメータ
	Params params_{};

	// 状態ごとの更新処理
	std::array<std::function<void(float deltaTime, bool jumpTriggered)>, kStateCount> stateUpdaters_;

public: // accessor

	float GetVelocityY() const { return velocityY_; }
	State GetState() const { return state_; }
	bool  IsGrounded() const { return state_ == State::Grounded; }
	const std::string& GetStateName() const;
	// そのフレームにジャンプが成立したか
	bool IsJumpStarted() const { return jumpStarted_; }

};

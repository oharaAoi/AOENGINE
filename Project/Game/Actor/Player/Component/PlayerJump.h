#pragma once
#include <array>
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
		float hangTime;
		float riseGravity;
		float fallGravity;
		float maxFallSpeed;
	};

	PlayerJump();
	~PlayerJump() = default;

	// 更新、着地
	void Update(float deltaTime, bool jumpTriggered, const Params& params);
	void Land();

private:
	std::size_t ToIndex(State state) const { return static_cast<std::size_t>(state); }
	void ChangeState(State next);
private:

	// 状態の数
	static constexpr std::size_t kStateCount = 4;

	State state_ = State::Grounded;
	float velocityY_ = 0.0f;
	float hangTimer_ = 0.0f;

	// パラメータ
	Params params_{};

	// 状態ごとの更新処理
	std::array<std::function<void(float deltaTime, bool jumpTriggered)>, kStateCount> stateUpdaters_;

public: // accessor

	float GetVelocityY() const { return velocityY_; }
	State GetState() const { return state_; }
	bool  IsGrounded() const { return state_ == State::Grounded; }
	const std::string& GetStateName() const;

};

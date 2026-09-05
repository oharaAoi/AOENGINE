#pragma once

/// <summary>
/// プレイヤーの入力を担当するコンポーネント
/// </summary>
class PlayerInput {
public:

	PlayerInput() = default;
	~PlayerInput() = default;

	// 更新
	void Update(float stickDeadZone);

private:

	float horizontal_ = 0.0f;
	bool  jumpTriggered_ = false;
	bool  jumpHeld_ = false;
	bool  launchTriggered_ = false;

public: // accessor


	float GetHorizontal() const { return horizontal_; }
	bool IsJumpTriggered() const { return jumpTriggered_; }
	// ジャンプ入力が押しっぱなしか
	bool IsJumpHeld() const { return jumpHeld_; }
	// 集めたブロックを打ち上げる入力が押されたか
	bool IsLaunchTriggered() const { return launchTriggered_; }

};

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

public: // accessor


	float GetHorizontal() const { return horizontal_; }
	bool IsJumpTriggered() const { return jumpTriggered_; }

};

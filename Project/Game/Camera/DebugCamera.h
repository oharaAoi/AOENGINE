#pragma once
#include "Game/Camera/BaseCamera.h"
#include "Engine/System/Input/Input.h"
#include "Engine/Lib/Math/Quaternion.h"
#include "Engine/Module/Components/Attribute/AttributeGui.h"

const float kDebugCameraMoveSpeed_ = 0.05f;

class DebugCamera :
	public BaseCamera,
	public AttributeGui {
public:

	DebugCamera();
	~DebugCamera() override;

	void Finalize() override;
	void Init() override;
	void Update() override;

	void Debug_Gui() override;

public:

	void SetIsFocused(bool isFocesd) { isFocused_ = isFocesd; }
	bool GetIsFocused() const { return isFocused_; }

	bool GetIsActive() const { return isActive_; }

private:

	/// <summary>
	/// カメラを動かす
	/// </summary>
	void TransitionMove();

	/// <summary>
	/// カメラを回転させる
	/// </summary>
	void RotateMove();

private:

	const float kCameraDeltaTime_ = 1.0f / GameTimer::fps_;

	Quaternion moveRotate_;
	// 回転する前のQuaternion
	Quaternion preMoveRotate_;

	// ---------------------------------------------------------------
	// ↓ デバックカメラで使う変数
	// ---------------------------------------------------------------
	
	float moveBaseSpeed_;
	float moveSpeed_;
	float moveMaxSpeed_ = 30.0f;
	Vector3 moveDirection_;
	Vector2 preMousePos_;

	float yaw_ = 0.0f;
	float pitch_ = 0.0f;
	float sensitivity_ = 0.05f; // 回転感度

	bool isMove = false;

	Quaternion qYaw;
	Quaternion qPitch;

	bool isFocused_;

	bool isActive_ = false;
};

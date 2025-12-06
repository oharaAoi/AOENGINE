#pragma once
#include "Game/Camera/BaseCamera.h"
#include "Engine/Lib/GameTimer.h"
#include "Engine/Lib/Math/Quaternion.h"
#include "Engine/Module/Components/Attribute/AttributeGui.h"

const float kDebugCameraMoveSpeed_ = 0.05f;

/// <summary>
/// Debugカメラ
/// </summary>
class DebugCamera :
	public BaseCamera,
	public AttributeGui {
public:

	DebugCamera();
	~DebugCamera() override;

public:

	// 終了処理
	void Finalize() override;
	// 初期化
	void Init() override;
	// 更新
	void Update() override;
	// 編集
	void Debug_Gui() override;

public:

	void SetIsFocused(bool isFocesd) { isFocused_ = isFocesd; }
	bool GetIsFocused() const { return isFocused_; }

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

	const float kCameraDeltaTime_ = 1.0f / AOENGINE::GameTimer::fps_;

	Math::Quaternion moveRotate_;
	// 回転する前のMath::Quaternion
	Math::Quaternion preMoveRotate_;

	// ---------------------------------------------------------------
	// ↓ デバックカメラで使う変数
	// ---------------------------------------------------------------
	
	float moveBaseSpeed_;
	float moveSpeed_;
	float moveMaxSpeed_ = 30.0f;
	Math::Vector3 moveDirection_;
	Math::Vector2 preMousePos_;

	float yaw_ = 0.0f;
	float pitch_ = 0.0f;
	float sensitivity_ = 0.05f; // 回転感度

	bool isMove = false;

	Math::Quaternion qYaw;
	Math::Quaternion qPitch;

	bool isFocused_;
};

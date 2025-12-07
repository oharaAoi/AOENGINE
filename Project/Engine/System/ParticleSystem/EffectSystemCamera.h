#pragma once
#include "Game/Camera/BaseCamera.h"
#include "Engine/Lib/Math/Quaternion.h"
#include "Engine/Module/Components/Attribute/AttributeGui.h"

namespace AOENGINE {

/// <summary>
/// Effectを作成する際に使用するカメラ
/// </summary>
class EffectSystemCamera :
	public BaseCamera,
	public AOENGINE::AttributeGui {
public:

	EffectSystemCamera();
	~EffectSystemCamera();

public:

	/// <summary>
	/// 終了処理
	/// </summary>
	void Finalize() override;

	/// <summary>
	/// 初期化処理
	/// </summary>
	void Init() override;

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update() override;

	/// <summary>
	/// 編集処理
	/// </summary>
	void Debug_Gui() override;

	/// <summary>
	/// カメラを動かす
	/// </summary>
	void TransitionMove();

	/// <summary>
	/// カメラを回転させる
	/// </summary>
	void RotateMove();

public:

	const Math::Matrix4x4 GetCameraMatrix() const { return cameraMatrix_; }
	const Math::Matrix4x4 GetViewMatrix() const { return viewMatrix_; }
	const Math::Matrix4x4 GetProjectionMatrix() const { return projectionMatrix_; }

	void SetIsFocused(bool isFocesd) { isFocused_ = isFocesd; }
	const bool GetIsFocused() const { return isFocused_; }

private:

	Math::Quaternion quaternion_;
	// 回転する前のMath::Quaternion
	Math::Quaternion moveQuaternion_;

	bool isFocused_;

	// ---------------------------------------------------------------
	// ↓ デバックカメラで使う変数
	// ---------------------------------------------------------------
	bool debugCameraMode_ = true;

	float moveBaseSpeed_;
	float moveSpeed_;
	float moveMaxSpeed_ = 30.0f;
	Math::Vector3 moveDirection_;
	Math::Vector2 preMousePos_;

	float yaw_ = 0.0f;
	float pitch_ = 0.0f;
	float sensitivity_ = 0.05f; // 回転感度

	Math::Quaternion qYaw;
	Math::Quaternion qPitch;
};

}
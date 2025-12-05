#pragma once
#include "Engine/Lib/Math/MyMatrix.h"
#include "Engine/Lib/Math/Vector2.h"

/// <summary>
/// 2dのカメラ
/// </summary>
class Camera2d {
public:

	Camera2d();
	~Camera2d();

public:

	// 初期化
	void Init();
	// 更新
	void Update();
	// 編集
	void Debug_Gui();

public:

	Math::Matrix4x4 GetViewMatrix() const { return viewMatrix_; }

	Math::Matrix4x4 GetProjectionMatrix() const { return projectionMatrix_; }

	Math::Matrix4x4 GetVPVMatrix() const { return worldMat_ * projectionMatrix_ * viewportMatrix_; }

private:

	float near_ = 0.0f;
	float far_ = 100.0f;

	Math::Matrix4x4 projectionMatrix_;
	Math::Matrix4x4 viewMatrix_;
	Math::Matrix4x4 viewportMatrix_;
	Math::Matrix4x4 worldMat_;

	Math::Vector3 translate_;
	
};


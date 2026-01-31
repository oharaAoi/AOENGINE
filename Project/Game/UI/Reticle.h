#pragma once
#include <memory>
#include "Engine/Module/Components/2d/Sprite.h"
#include <Lib/Math/Vector3.h>

/// <summary>
/// レティクル
/// </summary>
class Reticle {
public:

	Reticle() = default;
	~Reticle() = default;

public:

	// 初期化
	void Init();
	// 更新
	void Update();
	// 描画
	void Draw() const;
	// ロックオン
	void LockOn();

	void SetReticlePos(const Math::Matrix4x4& bossMat, const Math::Matrix4x4& vpvpMat);

public:		// accessor method

	Math::Vector3 GetTargetPos() const { return Math::Vector3(targetMat_.m[3][0], targetMat_.m[3][1], targetMat_.m[3][2]); }

	bool GetLockOn() const { return isLockOn_; }

	void ReleaseLockOn();

	const Math::Vector2& GetPos() const { return reticlePos_; }

private:

	AOENGINE::Sprite* reticle_;

	bool isLockOn_ = false;
	Math::Vector2 defaultPosition_;

	Math::Matrix4x4 targetMat_;

	Math::Vector2 reticlePos_;


};


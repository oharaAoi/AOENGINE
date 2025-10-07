#pragma once
#include <memory>
#include "Engine/Module/Components/2d/Sprite.h"
#include <Lib/Math/Vector3.h>

class Reticle {
public:

	Reticle() = default;
	~Reticle() = default;

	void Init();
	void Update(const Matrix4x4& bossMat, const Matrix4x4& vpvpMat);
	void Draw() const;

	void LockOn();

public:		// accessor method

	Vector3 GetTargetPos() const { return Vector3(targetMat_.m[3][0], targetMat_.m[3][1], targetMat_.m[3][2]); }

	bool GetLockOn() const { return isLockOn_; }

	void ReleaseLockOn();

	const Vector2& GetPos() const { return reticlePos_; }

private:

	Sprite* reticle_;

	bool isLockOn_ = false;
	Vector2 defaultPosition_;

	Matrix4x4 targetMat_;

	Vector2 reticlePos_;
};


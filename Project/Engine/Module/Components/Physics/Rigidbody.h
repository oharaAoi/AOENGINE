#pragma once
#include "Engine/Lib/Math/Vector3.h"

/// <summary>
/// 物理計算ようの
/// </summary>
class Rigidbody {
public:

	Rigidbody() { Init(); }
	~Rigidbody() = default;

	void Init();

	void Update();

public:

	const Vector3& GetMoveForce() const { return moveForce_; }

	void SetGravity(bool _isGravity) { isGravity_ = _isGravity; }

	const Vector3& GetPushbackForce() const { return pushbackForce_; }
	void SetPushbackForce(const Vector3& _force);

private:

	// 移動させる力
	Vector3 moveForce_;

	// 重力に関する変数
	bool isGravity_;
	Vector3 gravityAccel_;
	Vector3 gravityVelocity_;

	// 押し戻しに関する変数
	Vector3 pushbackForce_;

};


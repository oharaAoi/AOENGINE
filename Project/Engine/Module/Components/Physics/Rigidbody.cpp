#include "Rigidbody.h"
#include "Engine/Lib/Math/MyMath.h"
#include "Engine/Lib/GameTimer.h"

void Rigidbody::Init() {
	gravityAccel_ = { 0.0f, kGravity, 0.0f };
	gravityVelocity_ = CVector3::ZERO;
	moveForce_ = CVector3::ZERO;
}

void Rigidbody::Update() {
	moveForce_ = CVector3::ZERO;
	pushbackForce_ = CVector3::ZERO;
	// 重力の適応
	if (isGravity_) {
		gravityVelocity_ += gravityAccel_ * GameTimer::DeltaTime();
		moveForce_ += gravityVelocity_ * GameTimer::DeltaTime();
	} else {
		gravityVelocity_ = CVector3::ZERO;
	}
}

void Rigidbody::SetPushbackForce(const Vector3& _force) {
	pushbackForce_ += _force;

	if (isGravity_) {
		if (pushbackForce_.y > 0.0f) {
			gravityVelocity_.y = 0.0f;
		}
	}
}

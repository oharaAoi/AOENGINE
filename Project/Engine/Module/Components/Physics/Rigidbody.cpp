#include "Rigidbody.h"
#include "Engine/Lib/Math/MyMath.h"
#include "Engine/Lib/GameTimer.h"

void Rigidbody::Init() {
	gravityAccel_ = { 0.0f, kGravity, 0.0f };
	gravityVelocity_ = CVector3::ZERO;
	moveForce_ = CVector3::ZERO;
	velocity_ = CVector3::ZERO;
	drag_ = 0;
}

void Rigidbody::Update() {
	float dt = GameTimer::DeltaTime();

	float dragFactor = 1.0f / (1.0f + drag_ * dt);
	velocity_ *= dragFactor;

	moveForce_ = CVector3::ZERO;
	pushbackForce_ = CVector3::ZERO;
	// 重力の適応
	if (isGravity_) {
		gravityVelocity_ += gravityAccel_ * dt;
		moveForce_ += gravityVelocity_ * dt;
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

#include "Rigidbody.h"
#include "Engine/Lib/Math/MyMath.h"
#include "Engine/Lib/GameTimer.h"

using namespace AOENGINE;

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void Rigidbody::Init() {
	gravityAccel_ = { 0.0f, kGravity, 0.0f };
	moveForce_ = CVector3::ZERO;
	velocity_ = CVector3::ZERO;
	drag_ = 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void Rigidbody::Update() {
	float dt = AOENGINE::GameTimer::DeltaTime();

	float dragFactor = 1.0f / (1.0f + drag_ * dt);
	velocity_.x *= dragFactor;
	velocity_.z *= dragFactor;

	moveForce_ = CVector3::ZERO;
	pushbackForce_ = CVector3::ZERO;
	// 重力の適応
	if (isGravity_) {
		velocity_ += gravityAccel_ * dt;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集処理
///////////////////////////////////////////////////////////////////////////////////////////////

void Rigidbody::Debug_Gui() {
	ImGui::DragFloat3("gravityAccele", &gravityAccel_.x);
	ImGui::DragFloat3("moveForce_", &moveForce_.x);
	ImGui::DragFloat3("velocity", &velocity_.x);
	ImGui::BulletText("pushBackForce");
	ImGui::Text("x(%f), y(%f), z(%f),", pushbackForce_.x, pushbackForce_.y, pushbackForce_.z);
	ImGui::DragFloat("drag_", &drag_);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ velocity方向のMath::Quaternionを返す
///////////////////////////////////////////////////////////////////////////////////////////////

Math::Quaternion Rigidbody::LookVelocity(const Math::Quaternion& _rotate, float _rotateT, const Math::Vector3& _axis) {
	float angle = std::atan2f(velocity_.x, velocity_.z);
	Math::Quaternion lerpQuaternion = Math::Quaternion::Slerp(_rotate, Math::Quaternion::AngleAxis(angle, _axis), _rotateT);
	return lerpQuaternion;
}

void Rigidbody::SetPushbackForce(const Math::Vector3& _force) {
	pushbackForce_ += _force;

	if (isGravity_) {
		if (pushbackForce_.y > 0.0f) {
			velocity_.y = 0.0f;
		}
	}
}

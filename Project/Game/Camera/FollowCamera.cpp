#include "FollowCamera.h"
#include "Engine/Render.h"
#include "Engine/System/Editer/Window/EditerWindows.h"
#include "Engine/Lib/Json/JsonItems.h"
#include "Engine/Lib/Math/MyRandom.h"
#include "Engine/Lib/Math/MyMath.h"
#include "Engine/Lib/Math/Easing.h"
#include "Game/Actor/Player/Player.h"

void FollowCamera::Finalize() {
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集
///////////////////////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
void FollowCamera::Debug_Gui() {
	ImGui::Text("stick.Length: %f", stick_.Length());
	Vector3 pos = transform_.worldMat_.GetPosition();
	ImGui::DragFloat3("pos", &pos.x, 0.1f);
	ImGui::DragFloat2("angle", &angle_.x, 0.1f);
	ImGui::DragFloat("rotateLength_", &rotateLength_, 0.1f);

	if (ImGui::CollapsingHeader("Base", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::DragFloat("near", &near_, 0.1f);
		ImGui::DragFloat("far", &far_, 0.1f);
		ImGui::DragFloat("fovY", &fovY_, 0.1f);
	}

	if (ImGui::CollapsingHeader("FollowCamera", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::DragFloat("distance", &followCamera_.distance, 0.1f);
		ImGui::DragFloat("rotateDelta", &followCamera_.rotateDelta, 0.1f);
		ImGui::DragFloat3("offset", &followCamera_.offset.x, 0.1f);
		ImGui::DragFloat("complement", &followCamera_.complement, 0.01f);

		ImGui::DragFloat("followHeight", &followCamera_.followHeight, 0.01f);

		ImGui::DragFloat("limitMinY", &followCamera_.limitMinY, 0.01f);
		ImGui::DragFloat("limitMaxY", &followCamera_.limitMaxY, 0.01f);

		ImGui::DragFloat("smoothTime", &followCamera_.smoothTime, 0.01f);
		ImGui::DragFloat("maxSpeed", &followCamera_.maxSpeed, 0.01f);
		SelectEasing(followCamera_.easingIndex);

		followCamera_.complement = std::clamp(followCamera_.complement, 0.0f, 1.0f);
		followCamera_.smoothTime = std::clamp(followCamera_.smoothTime, 0.001f, 100.0f);
		followCamera_.maxSpeed = std::clamp(followCamera_.maxSpeed, 0.0f, 1000000.0f);

		if (ImGui::Button("Save")) {
			JsonItems::Save("FollowCamera", followCamera_.ToJson("FollowCamera"));
		}
		if (ImGui::Button("Apply")) {
			followCamera_.FromJson(JsonItems::GetData("FollowCamera", "FollowCamera"));
		}
	}

	projectionMatrix_ = Matrix4x4::MakePerspectiveFov(fovY_, float(kWindowWidth_) / float(kWindowHeight_), near_, far_);
}
#endif // _DEBUG

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化
///////////////////////////////////////////////////////////////////////////////////////////////

void FollowCamera::Init() {
	BaseCamera::Init();

	followCamera_.FromJson(JsonItems::GetData("FollowCamera", "FollowCamera"));

	shakeTimer_ = shakeTime_;

	//angle_.y = 30.0f * kToRadian;

	transform_.rotate = Quaternion(0, 0, 0, 1.0f);

	pivotSRT_.scale = { 1.0f, 1.0f, 1.0f };
	pivotSRT_.rotate = Quaternion(0, 0, 0, 1.0f);
	transform_.SetParent(pivotSRT_.worldMat_);

#ifdef _DEBUG
	EditerWindows::AddObjectWindow(this, "FollowCamera");
#endif // _DEBUG
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新
///////////////////////////////////////////////////////////////////////////////////////////////

void FollowCamera::Update() {
	if (!pTarget_) {
		return;
	}

	Vector3 targetPos = pTarget_->GetPosition();
	
	InputStick();

	RotateCamera();

	MoveCamera(targetPos);

	Shake();

	pivotSRT_.worldMat_ = pivotSRT_.MakeAffine();
	transform_.worldMat_ = transform_.MakeAffine();
	BaseCamera::Update();

	// renderの更新
	Render::SetEyePos(GetWorldPosition());
	Render::SetViewProjection(viewMatrix_, projectionMatrix_);
	Render::SetCameraRotate(pivotSRT_.rotate);
}


///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ カメラを回転量を取得する
///////////////////////////////////////////////////////////////////////////////////////////////

void FollowCamera::InputStick() {
	stick_ = Input::GetInstance()->GetRightJoyStick(kDeadZone_).Normalize();
	if (std::abs(stick_.x) > kDeadZone_) {
		angle_.x += stick_.x * followCamera_.rotateDelta * GameTimer::DeltaTime();
	}

	if (std::abs(stick_.y) > kDeadZone_) {
		angle_.y -= stick_.y * followCamera_.rotateDelta * GameTimer::DeltaTime();
	}

	float limitMinY = -kPI / followCamera_.limitMinY;
	float limitMaxY = kPI / followCamera_.limitMaxY;
	angle_.y = std::clamp(angle_.y, limitMinY, limitMaxY);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ カメラを回転させる
///////////////////////////////////////////////////////////////////////////////////////////////

void FollowCamera::RotateCamera() {
	if (pReticle_->GetLockOn()) {
		pivotSRT_.rotate = Quaternion::LookAt(pTarget_->GetPosition(), pReticle_->GetTargetPos());

		Vector3 euler = pivotSRT_.rotate.QuaternionToEuler();
		angle_.x = euler.y;
		angle_.y = euler.x;

	} else {
		Quaternion yaw = Quaternion::AngleAxis(angle_.x, CVector3::UP);
		Quaternion pitch = Quaternion::AngleAxis(angle_.y, CVector3::RIGHT);
		pivotSRT_.rotate = yaw * pitch;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ カメラを移動させる
///////////////////////////////////////////////////////////////////////////////////////////////

void FollowCamera::MoveCamera(const Vector3& target) {
	pivotSRT_.translate = SmoothDamp(prePosition_, target, velocity_, followCamera_.smoothTime, followCamera_.maxSpeed, GameTimer::DeltaTime());
	transform_.translate = followCamera_.offset;
	prePosition_ = pivotSRT_.translate;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ カメラシェイク
///////////////////////////////////////////////////////////////////////////////////////////////

void FollowCamera::Shake() {
	if (shakeTimer_ <= shakeTime_) {
		shakeTimer_ += GameTimer::DeltaTime();
		Vector3 shakeDire = RandomVector3(CVector3::UNIT * -1, CVector3::UNIT).Normalize();

		float t = shakeTimer_ / shakeTime_;
		float currentShakeStrength_ = std::lerp(shakeStrength_, 0.0f, t);
		shakeDire *= currentShakeStrength_;
		transform_.translate += shakeDire;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ accessor
///////////////////////////////////////////////////////////////////////////////////////////////

void FollowCamera::SetShake(float time, float strength) {
	shakeTime_ = time;
	shakeTimer_ = 0;
	shakeStrength_ = strength;
}

void FollowCamera::SetTarget(Player* _target) {
	pTarget_ = _target;
	//Quaternion rotation = Quaternion::EulerToQuaternion(angle_.y, angle_.x, 0.0f);
	//Vector3 offset = rotation * Vector3(0.0f, 0.0f, followCamera_.distance);
	//Vector3 desiredPosition = pTarget_->GetTransform()->translate_ + (offset * -1);
	//transform_.translate = desiredPosition;
	//pivotSRT_.LookAt(pTarget_->GetTransform()->translate_);
}

Quaternion FollowCamera::GetAngleX() {
	//return Quaternion::AngleAxis(angle_.x, CVector3::UP);
	Vector3 euler = pivotSRT_.rotate.QuaternionToEuler();
	return Quaternion::AngleAxis(euler.y, CVector3::UP);
}

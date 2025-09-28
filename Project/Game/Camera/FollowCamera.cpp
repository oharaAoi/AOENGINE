#include "FollowCamera.h"
#include "Engine/Render.h"
#include "Engine/System/Editer/Window/EditorWindows.h"
#include "Engine/System/Input/Input.h"
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

void FollowCamera::Debug_Gui() {
	ImGui::Text("stick.Length: %f", stick_.Length());
	Vector3 pos = transform_.worldMat_.GetPosition();
	ImGui::DragFloat3("pos", &pos.x, 0.1f);
	ImGui::DragFloat3("angle", &angle_.x, 0.1f);
	ImGui::DragFloat("rotateLength_", &rotateLength_, 0.1f);

	if (ImGui::CollapsingHeader("Base")) {
		ImGui::DragFloat("near", &near_, 0.1f);
		ImGui::DragFloat("far", &far_, 0.1f);
		ImGui::DragFloat("fovY", &fovY_, 0.1f);
	}

	if (ImGui::CollapsingHeader("FollowCamera")) {
		followCamera_.Debug_Gui();

		followCamera_.complement = std::clamp(followCamera_.complement, 0.0f, 1.0f);
		followCamera_.smoothTime = std::clamp(followCamera_.smoothTime, 0.001f, 100.0f);
		followCamera_.maxSpeed = std::clamp(followCamera_.maxSpeed, 0.0f, 1000000.0f);
	}

	if (ImGui::CollapsingHeader("AnimationParam")) {
		animationParam_.Debug_Gui();

		if (ImGui::Button("Reset")) {
			isAnimationFinish_ = false;
			animationTimer_ = 0.0f;
			grayscale_->SetIsEnable(true);
			vignette_->SetIsEnable(true);
		}
	}

	projectionMatrix_ = Matrix4x4::MakePerspectiveFov(fovY_, float(kWindowWidth_) / float(kWindowHeight_), near_, far_);
}

void FollowCamera::CameraParameter::Debug_Gui() {
	ImGui::DragFloat("distance", &distance, 0.1f);
	ImGui::DragFloat("rotateDelta", &rotateDelta, 0.1f);
	ImGui::DragFloat3("offset", &offset.x, 0.1f);
	ImGui::DragFloat("complement", &complement, 0.01f);

	ImGui::DragFloat("followHeight", &followHeight, 0.01f);

	ImGui::DragFloat("limitMinY", &limitMinY, 0.01f);
	ImGui::DragFloat("limitMaxY", &limitMaxY, 0.01f);

	ImGui::DragFloat("smoothTime", &smoothTime, 0.01f);
	ImGui::DragFloat("maxSpeed", &maxSpeed, 0.01f);
	SelectEasing(easingIndex);
	SaveAndLoad();
}

void FollowCamera::AnimationParameter::Debug_Gui() {
	ImGui::DragFloat3("firstOffset", &firstOffset.x, 0.1f);
	ImGui::DragFloat("animationTime", &moveTime, 0.1f);
	SelectEasing(easingIndex);
	ImGui::ColorEdit4("color", &scaleColor.x);
	ImGui::DragFloat("vginetteColor", &vignettePower, 0.01f);
	SaveAndLoad();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化
///////////////////////////////////////////////////////////////////////////////////////////////

void FollowCamera::Init() {
	BaseCamera::Init();

	followCamera_.Load();
	animationParam_.Load();

	animationTimer_ = 0.0f;

	shakeTimer_ = shakeTime_;
	animationParam_.targetOffset = followCamera_.offset;
	isAnimationFinish_ = false;

	transform_.rotate = Quaternion(0, 0, 0, 1.0f);

	pivotSRT_.scale = { 1.0f, 1.0f, 1.0f };
	pivotSRT_.rotate = Quaternion(0, 0, 0, 1.0f);
	transform_.SetParent(pivotSRT_.worldMat_);

	grayscale_ = Engine::GetPostProcess()->GetGrayscale();
	vignette_ = Engine::GetPostProcess()->GetVignette();
	grayscale_->SetIsEnable(true);
	vignette_->SetIsEnable(true);

	Input::SetNotAccepted(true);

#ifdef _DEBUG
	EditorWindows::AddObjectWindow(this, "FollowCamera");
#endif // _DEBUG
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新
///////////////////////////////////////////////////////////////////////////////////////////////

void FollowCamera::Update() {
	if (!pTarget_) {
		return;
	}

	if (!isAnimationFinish_) {
		FirstCameraMove();
	}

	Vector3 targetPos = pTarget_->GetGameObject()->GetPosition();
	
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
		pivotSRT_.rotate = Quaternion::LookAt(pTarget_->GetGameObject()->GetPosition(), pReticle_->GetTargetPos());
		Quaternion roll = Quaternion::AngleAxis(angle_.z, CVector3::FORWARD);
		pivotSRT_.rotate = pivotSRT_.rotate * roll;

		Vector3 euler = pivotSRT_.rotate.QuaternionToEuler();
		angle_.x = euler.y;
		angle_.y = euler.x;
		angle_.z = euler.z;

	} else {
		Quaternion yaw = Quaternion::AngleAxis(angle_.x, CVector3::UP);
		Quaternion pitch = Quaternion::AngleAxis(angle_.y, CVector3::RIGHT);
		Quaternion roll = Quaternion::AngleAxis(angle_.z, CVector3::FORWARD);
		pivotSRT_.rotate = yaw * pitch * roll;
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

void FollowCamera::FirstCameraMove() {
	animationTimer_ += GameTimer::DeltaTime();
	float t = animationTimer_ / animationParam_.moveTime;
	followCamera_.offset = Vector3::Lerp(animationParam_.firstOffset, animationParam_.targetOffset, CallEasing(animationParam_.easingIndex, t));

	// grayscale分
	float alpha = std::lerp(1.0f, 0.0f, t);
	animationParam_.scaleColor.w = alpha;
	grayscale_->SetColor(animationParam_.scaleColor);

	// vignette分
	float power = std::lerp(animationParam_.vignettePower, 0.0f, t);
	vignette_->SetPower(power);

	if (animationTimer_ > animationParam_.moveTime) {
		grayscale_->SetIsEnable(false);
		vignette_->SetIsEnable(false);
		isAnimationFinish_ = true;

		Input::SetNotAccepted(false);
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
	//Vector3 desiredPosition = pTarget_->GetTransform()->srt_.translate + (offset * -1);
	//transform_.translate = desiredPosition;
	//pivotSRT_.LookAt(pTarget_->GetTransform()->srt_.translate);
}

Quaternion FollowCamera::GetAngleX() {
	//return Quaternion::AngleAxis(angle_.x, CVector3::UP);
	Vector3 euler = pivotSRT_.rotate.QuaternionToEuler();
	return Quaternion::AngleAxis(euler.y, CVector3::UP);
}

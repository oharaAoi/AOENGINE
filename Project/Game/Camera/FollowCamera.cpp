#include "FollowCamera.h"
#include "Engine/Render.h"
#include "Engine/System/Editor/Window/EditorWindows.h"
#include "Engine/System/Input/Input.h"
#include "Engine/Lib/Math/MyRandom.h"
#include "Engine/Lib/Math/MyMath.h"
#include "Engine/Lib/Math/Easing.h"
#include "Engine/WinApp/WinApp.h"
#include "Engine/Lib/GameTimer.h"
#include "Game/Actor/Player/Player.h"
#include "Game/Camera/Animation/CameraAnimationShot.h"
#include "Game/Camera/Animation/CameraAnimationBoost.h"

FollowCamera::FollowCamera() = default;

void FollowCamera::Finalize() {
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集
///////////////////////////////////////////////////////////////////////////////////////////////

void FollowCamera::Debug_Gui() {
	ImGui::Text("stick.Length: %f", stick_.Length());
	Math::Vector3 pos = transform_.worldMat_.GetPosition();
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

	// animationの変更
	for (auto& animation : animationMap_) {
		animation.second->Debug_Gui();
	}
	
	projectionMatrix_ = Math::Matrix4x4::MakePerspectiveFov(fovY_, float(AOENGINE::WinApp::sClientWidth) / float(AOENGINE::WinApp::sClientHeight), near_, far_);
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
	Math::SelectEasing(easingIndex, "Camera");
	SaveAndLoad();
}

void FollowCamera::AnimationParameter::Debug_Gui() {
	ImGui::DragFloat3("firstOffset", &firstOffset.x, 0.1f);
	ImGui::DragFloat("animationTime", &moveTime, 0.1f);
	Math::SelectEasing(easingIndex, "AnimationCamera");
	ImGui::ColorEdit4("color", &scaleColor.r);
	ImGui::DragFloat("vginetteColor", &vignettePower, 0.01f);
	SaveAndLoad();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化
///////////////////////////////////////////////////////////////////////////////////////////////

void FollowCamera::Init() {
	BaseCamera::Init();

	initFollowCamera_.Load();
	animationParam_.Load();
	followCamera_ = initFollowCamera_;

	animationTimer_ = 0.0f;

	shakeTimer_ = shakeTime_;
	animationParam_.targetOffset = followCamera_.offset;
	isAnimationFinish_ = false;

	transform_.rotate = Math::Quaternion(0, 0, 0, 1.0f); 

	pivotSRT_.scale = { 1.0f, 1.0f, 1.0f };
	pivotSRT_.rotate = Math::Quaternion(0, 0, 0, 1.0f);
	transform_.SetParent(pivotSRT_.worldMat_);

	grayscale_ = Engine::GetPostProcess()->GetEffectAs<PostEffect::Grayscale>(PostEffectType::Grayscale);
	vignette_ = Engine::GetPostProcess()->GetEffectAs<PostEffect::Vignette>(PostEffectType::Vignette);

	grayscale_->SetIsEnable(true);
	vignette_->SetIsEnable(true);

	AOENGINE::Input::SetNotAccepted(true);

	animationMap_["shotAnimation"] = std::make_unique<CameraAnimationShot>();
	animationMap_["boostAnimation"] = std::make_unique<CameraAnimationBoost>();
 
	for (auto& animation : animationMap_) {
		animation.second->Init();
		animation.second->SetFollowCamera(this);
	}

	prePosition_ = CVector3::ZERO;
	velocity_ = CVector3::ZERO;

#ifdef _DEBUG
	AOENGINE::EditorWindows::AddObjectWindow(this, "FollowCamera");
#endif // _DEBUG
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新
///////////////////////////////////////////////////////////////////////////////////////////////

void FollowCamera::Update() {
	if (!pTarget_) {
		return;
	}

	// 最初のカメラの動き
	if (!isAnimationFinish_) {
		FirstCameraMove();
	}

	// animationの更新
	for (auto& animation : animationMap_) {
		animation.second->Update();
	}

	Math::Vector3 targetPos = pTarget_->GetGameObject()->GetPosition();
	
	// パッドのスティクを入力
	InputStick();

	// カメラの回転
	RotateCamera();

	// カメラの移動
	MoveCamera(targetPos);

	// カメラのシェイク
	Shake();

	pivotSRT_.worldMat_ = pivotSRT_.MakeAffine();
	transform_.worldMat_ = transform_.MakeAffine();
	BaseCamera::Update();

	// renderの更新
	AOENGINE::Render::SetEyePos(GetWorldPosition());
	AOENGINE::Render::SetViewProjection(viewMatrix_, projectionMatrix_);
	AOENGINE::Render::SetCameraRotate(pivotSRT_.rotate);
}


///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ カメラを回転量を取得する
///////////////////////////////////////////////////////////////////////////////////////////////

void FollowCamera::InputStick() {
	stick_ = AOENGINE::Input::GetInstance()->GetRightJoyStick(kDeadZone_).Normalize();
	if (std::abs(stick_.x) > kDeadZone_) {
		angle_.x += stick_.x * followCamera_.rotateDelta * AOENGINE::GameTimer::DeltaTime();
	}

	if (std::abs(stick_.y) > kDeadZone_) {
		angle_.y -= stick_.y * followCamera_.rotateDelta * AOENGINE::GameTimer::DeltaTime();
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
		pivotSRT_.rotate = Math::Quaternion::LookAt(pTarget_->GetGameObject()->GetPosition(), pReticle_->GetTargetPos());
		Math::Quaternion roll = Math::Quaternion::AngleAxis(angle_.z, CVector3::FORWARD);
		pivotSRT_.rotate = pivotSRT_.rotate * roll;

		Math::Vector3 euler = pivotSRT_.rotate.QuaternionToEuler();
		angle_.x = euler.y;
		angle_.y = euler.x;
		angle_.z = euler.z;

	} else {
		Math::Quaternion yaw = Math::Quaternion::AngleAxis(angle_.x, CVector3::UP);
		Math::Quaternion pitch = Math::Quaternion::AngleAxis(angle_.y, CVector3::RIGHT);
		Math::Quaternion roll = Math::Quaternion::AngleAxis(angle_.z, CVector3::FORWARD);
		pivotSRT_.rotate = yaw * pitch * roll;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ カメラを移動させる
///////////////////////////////////////////////////////////////////////////////////////////////

void FollowCamera::MoveCamera(const Math::Vector3& target) {
	pivotSRT_.translate = SmoothDamp(prePosition_, target, velocity_, followCamera_.smoothTime, followCamera_.maxSpeed, AOENGINE::GameTimer::DeltaTime());
	transform_.translate = followCamera_.offset;
	prePosition_ = pivotSRT_.translate;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ カメラシェイク
///////////////////////////////////////////////////////////////////////////////////////////////

void FollowCamera::Shake() {
	if (shakeTimer_ <= shakeTime_) {
		shakeTimer_ += AOENGINE::GameTimer::DeltaTime();
		Math::Vector3 shakeDire = Random::RandomVector3(CVector3::UNIT * -1, CVector3::UNIT).Normalize();

		float t = shakeTimer_ / shakeTime_;
		float currentShakeStrength_ = std::lerp(shakeStrength_, 0.0f, t);
		shakeDire *= currentShakeStrength_;
		transform_.translate += shakeDire;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 最初のカメラ移動
///////////////////////////////////////////////////////////////////////////////////////////////

void FollowCamera::FirstCameraMove() {
	animationTimer_ += AOENGINE::GameTimer::DeltaTime();
	float t = animationTimer_ / animationParam_.moveTime;
	followCamera_.offset = Math::Vector3::Lerp(animationParam_.firstOffset, animationParam_.targetOffset, Math::CallEasing(animationParam_.easingIndex, t));

	// grayscale分
	AOENGINE::Color color = AOENGINE::Color::Lerp(animationParam_.scaleColor, Colors::Linear::white, t);
	grayscale_->SetColor(color);

	// vignette分
	float power = std::lerp(animationParam_.vignettePower, 0.0f, t);
	vignette_->SetPower(power);

	if (animationTimer_ > animationParam_.moveTime) {
		grayscale_->SetIsEnable(false);
		vignette_->SetIsEnable(false);
		isAnimationFinish_ = true;

		AOENGINE::Input::SetNotAccepted(false);
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
}

Math::Quaternion FollowCamera::GetAngleX() {
	Math::Vector3 euler = pivotSRT_.rotate.QuaternionToEuler();
	return Math::Quaternion::AngleAxis(euler.y, CVector3::UP);
}

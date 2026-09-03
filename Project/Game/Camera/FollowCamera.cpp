#include "FollowCamera.h"

#include <cmath>

#include "Engine/Lib/Math/MyMath.h"
#include "Engine/Lib/GameTimer.h"
#include "Engine/System/Manager/ImGuiManager.h"
#include "Engine/Module/Components/GameObject/BaseGameObject.h"
#include "Engine/Module/Components/WorldTransform.h"
#include "Engine/Utilities/SceneObjectFinder.h"

///////////////////////////////////////////////////////////////////////////////////////////////
//  初期化
///////////////////////////////////////////////////////////////////////////////////////////////

void FollowCamera::Init() {
	BaseCamera::Init();
	SetName("followCamera");

	// 保存済みの調整値を読み込む
	parameter_.Load();

	target_ = nullptr;

	followVelocity_ = CVector3::ZERO;
	smoothedTarget_ = CVector3::ZERO;
	initialized_ = false;

	cameraTargetY_ = 0.0f;
	isScrolling_ = false;

	shakeTime_ = 0.0f;
	shakeTimer_ = 0.0f;
	shakeStrength_ = 0.0f;
	shakeOffset_ = CVector3::ZERO;

	ResolveTarget();
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  更新
///////////////////////////////////////////////////////////////////////////////////////////////

void FollowCamera::Update() {
	const float deltaTime = AOENGINE::GameTimer::DeltaTime();

	if (!target_) {
		ResolveTarget();
	}

	if (target_) {
		FollowTarget(deltaTime);
	}
	UpdateShake(deltaTime);

	// 補間したターゲット + オフセット + シェイクオフセット
	transform_.translate = smoothedTarget_ + parameter_.offset + shakeOffset_;

	// 少し見下ろす向きに固定
	transform_.rotate = Math::Quaternion::AngleAxis(parameter_.pitch * kToRadian, CVector3::RIGHT);

	// view / projection 行列を作り直す
	BaseCamera::Update();
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  追従対象の取得
///////////////////////////////////////////////////////////////////////////////////////////////

void FollowCamera::ResolveTarget() {
	AOENGINE::BaseGameObject* found = FindSceneObject<AOENGINE::BaseGameObject>(targetName_);
	if (found && found != target_) {
		target_ = found;
		initialized_ = false;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  ターゲット追従
///////////////////////////////////////////////////////////////////////////////////////////////

void FollowCamera::FollowTarget(float deltaTime) {
	const Math::Vector3 targetPos = target_->GetTransform()->GetTranslate();

	if (!initialized_) {
		//初期化
		smoothedTarget_ = targetPos;
		followVelocity_ = CVector3::ZERO;
		cameraTargetY_ = targetPos.y;
		isScrolling_ = false;
		initialized_ = true;
		return;
	}

	// スクロール中はスクロール判定しない
	if (!isScrolling_) {
		const Math::Matrix4x4 viewProjection = GetViewMatrix() * GetProjectionMatrix();
		const Math::Vector3 screenPos = TransformCoord(targetPos, viewProjection);

		if (screenPos.y >= 0.0f) {
			cameraTargetY_ = smoothedTarget_.y + parameter_.scrollHeight;
			isScrolling_ = true;
		}
	}

	if (!isScrolling_) {
		return;
	}

	// 縦だけスクロール先へイージングする
	Math::Vector3 scrollTarget = smoothedTarget_;
	scrollTarget.y = cameraTargetY_;

	smoothedTarget_ = SmoothDamp(
		smoothedTarget_, scrollTarget, followVelocity_,
		parameter_.smoothTime, parameter_.maxSpeed, deltaTime);

	// 目標の高さまで届いたらスクロール終了
	if (std::abs(cameraTargetY_ - smoothedTarget_.y) < kScrollArriveThreshold) {
		smoothedTarget_.y = cameraTargetY_;
		followVelocity_.y = 0.0f;
		isScrolling_ = false;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  シェイク
///////////////////////////////////////////////////////////////////////////////////////////////

void FollowCamera::SetShake(float time, float strength) {
	shakeTime_ = time;
	shakeTimer_ = 0.0f;
	shakeStrength_ = strength;
}

void FollowCamera::UpdateShake(float deltaTime) {
	shakeOffset_ = CVector3::ZERO;

	if (shakeTimer_ >= shakeTime_) {
		return;
	}

	// タイマー更新
	shakeTimer_ += deltaTime;

	// 時間で減衰させる
	const float t = 1.0f - std::clamp(shakeTimer_ / shakeTime_, 0.0f, 1.0f);
	const float amplitude = shakeStrength_ * t;

	shakeOffset_ = Math::Vector3(
		std::sin(shakeTimer_ * parameter_.shakeFrequency.x) * amplitude,
		std::sin(shakeTimer_ * parameter_.shakeFrequency.y) * amplitude,
		0.0f);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// デバッグ表示 
///////////////////////////////////////////////////////////////////////////////////////////////

void FollowCamera::Debug_Gui() {
	const Math::Vector3 pos = transform_.translate;

	const char* targetState = "null";
	if (target_) {
		targetState = targetName_.c_str();
	}

	const char* scrollState = "false";
	if (isScrolling_) {
		scrollState = "true";
	}

	ImGui::Text("target: %s", targetState);
	ImGui::Text("pos: %.2f, %.2f, %.2f", pos.x, pos.y, pos.z);
	ImGui::Text("scrolling: %s (goal Y: %.2f)", scrollState, cameraTargetY_);

	if (ImGui::Button("Test Shake")) {
		SetShake(0.3f, 0.5f);
	}
}

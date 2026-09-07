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

	// パラメータ、シェイクロード
	parameter_.Load();
	shakeRequest_.Load();

	// 初期値
	target_ = nullptr;
	followVelocity_ = CVector3::ZERO;
	smoothedTarget_ = CVector3::ZERO;
	cameraTargetY_ = 0.0f;
	initialized_ = false;
	isScrolling_ = false;

	// ターゲット決定
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
	// 追従
	if (target_) {
		FollowTarget(deltaTime);
	}
	// transform適用(追従ターゲット位置+普通のオフセット)
	transform_.translate = smoothedTarget_ + parameter_.offset;

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

	// リクエストが来ている間は直接追従モードに入る
	if (continuousFollowRequested_ && !continuousFollowActive_) {
		continuousFollowActive_ = true;
		followVelocity_ = CVector3::ZERO;
	}

	// 直接追従と段階スクロールは同時には動かない
	if (continuousFollowActive_) {
		UpdateContinuousFollow(targetPos, deltaTime);
		return;
	}

	UpdateStepScroll(targetPos, deltaTime);
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  直接追従
///////////////////////////////////////////////////////////////////////////////////////////////

void FollowCamera::UpdateContinuousFollow(const Math::Vector3& targetPos, float deltaTime) {

	// ダメージ床のノックバックで飛んでいる間は、プレイヤーへ直接イージングで追従する
	cameraTargetY_ = targetPos.y;

	Math::Vector3 followTarget = smoothedTarget_;
	followTarget.y = cameraTargetY_;

	//smoothedTargetの計算
	smoothedTarget_ = SmoothDamp(
		smoothedTarget_, followTarget, followVelocity_,
		parameter_.bigJumpSmoothTime, parameter_.bigJumpMaxSpeed, deltaTime);

	// リクエストが終わっていて、かつ実際に追いつききったら通常モードへ戻す
	const bool caughtUp = std::abs(targetPos.y - smoothedTarget_.y) < kScrollArriveThreshold;
	if (!continuousFollowRequested_ && caughtUp) {

		// 追いつききったので段階スクロールへ戻す
		smoothedTarget_.y = targetPos.y;
		followVelocity_ = CVector3::ZERO;
		continuousFollowActive_ = false;
		isScrolling_ = false;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  段階スクロール(通常時)
///////////////////////////////////////////////////////////////////////////////////////////////

void FollowCamera::UpdateStepScroll(const Math::Vector3& targetPos, float deltaTime) {

	// スクロール中はスクロール判定しない
	if (!isScrolling_) {
		const Math::Matrix4x4 viewProjection = GetViewMatrix() * GetProjectionMatrix();
		const Math::Vector3 screenPos = TransformCoord(targetPos, viewProjection);

		// 0から1を、NDCように-1から1の間になるようにする
		const float triggerNdcY = parameter_.scrollTriggerScreenY * 2.0f - 1.0f;

		// スクロール条件を超えたら上にスクロールする
		if (screenPos.y >= triggerNdcY) {
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

	// 揺れ方の項目はCameraShakeRequestが自前で描画する
	ImGui::PushID("CameraShake");
	shakeRequest_.Debug_Gui();
	shakeRequest_.SaveAndLoad();

	if (ImGui::Button("Test Play")) {
		PlayShake(shakeRequest_);
	}
	ImGui::SameLine();
	if (ImGui::Button("Stop")) {
		StopShake();
	}
	ImGui::SameLine();
	if (IsShaking()) {
		ImGui::TextUnformatted("Playing");
	} else {
		ImGui::TextUnformatted("Stopped");
	}
	ImGui::PopID();
}

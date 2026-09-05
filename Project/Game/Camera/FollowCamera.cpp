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
	// カメラシェイクはCustomParameterSetでは扱えない型なので、個別に読み込む
	shakeRequest_.Load();

	target_ = nullptr;

	followVelocity_ = CVector3::ZERO;
	smoothedTarget_ = CVector3::ZERO;
	initialized_ = false;

	cameraTargetY_ = 0.0f;
	isScrolling_ = false;

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
	// シェイクはBaseCameraが描画用姿勢へ適用する
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
	if (continuousFollowRequested_) {
		continuousFollowActive_ = true;
	}

	if (continuousFollowActive_) {
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
			// スクロール終わり
			continuousFollowActive_ = false;
			isScrolling_ = false;
		}
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

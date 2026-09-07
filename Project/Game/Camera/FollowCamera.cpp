#include "FollowCamera.h"

#include <algorithm>
#include <cmath>
#include <cstdint>

#include "Engine/Lib/Math/MyMath.h"
#include "Engine/Lib/GameTimer.h"
#include "Engine/System/Manager/ImGuiManager.h"
#include "Engine/Module/Components/GameObject/BaseGameObject.h"
#include "Engine/Module/Components/WorldTransform.h"
#include "Engine/Utilities/SceneObjectFinder.h"
#include "Engine/WinApp/WinApp.h"

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

	// 頂点を過ぎたかを見張る。ここで直接追従へ入るかが決まる
	UpdateApexWatch(targetPos);

	// 直接追従と段階スクロールは同時には動かない
	if (continuousFollowActive_) {
		UpdateContinuousFollow(targetPos, deltaTime);
		return;
	}

	UpdateStepScroll(targetPos, deltaTime);
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  頂点の見張り
///////////////////////////////////////////////////////////////////////////////////////////////

void FollowCamera::UpdateApexWatch(const Math::Vector3& targetPos) {

	// 飛んでいない間は比べる元だけ更新しておく
	if (!continuousFollowRequested_) {
		prevTargetY_ = targetPos.y;
		return;
	}

	// 既に入っていれば見張る必要はない
	if (continuousFollowActive_) {
		return;
	}

	// 上がっている間は段階スクロールに任せる。
	// 落ち始めた瞬間から直接追従へ切り替える
	if (targetPos.y < prevTargetY_) {
		continuousFollowActive_ = true;
		followVelocity_ = CVector3::ZERO;
	}

	prevTargetY_ = targetPos.y;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  直接追従
///////////////////////////////////////////////////////////////////////////////////////////////

void FollowCamera::UpdateContinuousFollow(const Math::Vector3& targetPos, float deltaTime) {

	// プレイヤーが画面の決まった高さに見えるところまでカメラを寄せる
	cameraTargetY_ = CalcCameraYForScreen(targetPos, parameter_.bigJumpScreenY);

	Math::Vector3 followTarget = smoothedTarget_;
	followTarget.y = cameraTargetY_;

	//smoothedTargetの計算
	smoothedTarget_ = SmoothDamp(
		smoothedTarget_, followTarget, followVelocity_,
		parameter_.bigJumpSmoothTime, parameter_.bigJumpMaxSpeed, deltaTime);

	// リクエストが終わっていて、かつ実際に追いつききったら通常モードへ戻す
	const bool caughtUp = std::abs(cameraTargetY_ - smoothedTarget_.y) < kScrollArriveThreshold;
	if (!continuousFollowRequested_ && caughtUp) {

		// 追いつききったので段階スクロールへ戻す
		smoothedTarget_.y = cameraTargetY_;
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
//  画面上の高さから、カメラのY座標を求める
///////////////////////////////////////////////////////////////////////////////////////////////

float FollowCamera::CalcCameraYForScreen(const Math::Vector3& targetPos, float screenNdcY) const {

	const float width = static_cast<float>((std::max<std::uint32_t>)(1u, AOENGINE::WinApp::sClientWidth));
	const float height = static_cast<float>((std::max<std::uint32_t>)(1u, AOENGINE::WinApp::sClientHeight));

	// スクリーン座標
	const Math::Vector2 screenPosition{ width * 0.5f, (1.0f - screenNdcY) * 0.5f * height };

	// 今のカメラで、その高さに見えるワールド座標を引く
	const Math::Matrix4x4 viewProjection = GetViewMatrix() * GetProjectionMatrix();
	const Math::Vector3 anchor = screenAnchor_.Solve(viewProjection, { screenPosition, targetPos.z });

	// そこへプレイヤーが来るように、ずれているぶんだけカメラを動かす
	return smoothedTarget_.y + (targetPos.y - anchor.y);
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
	}
	else {
		ImGui::TextUnformatted("Stopped");
	}
	ImGui::PopID();
}

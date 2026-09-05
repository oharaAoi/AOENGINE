#include "Boss.h"

#include "Engine/Module/Components/GameObject/BaseGameObject.h"
#include "Engine/Module/Components/WorldTransform.h"
#include "Engine/Lib/Math/Easing.h"
#include "Engine/System/Manager/ImGuiManager.h"

#include "Game/Camera/FollowCamera.h"
#include"Engine/Lib/GameTimer.h"

using namespace AOENGINE;

///////////////////////////////////////////////////////////////////////////////////////////////
//  初期化
///////////////////////////////////////////////////////////////////////////////////////////////

void Boss::Init(BaseGameObject* body) {
	Bind(body);

	// 保存済みの調整値を読み込む
	parameter_.Load();
	// カメラシェイクはCustomParameterSetでは扱えない型なので、個別に読み込む
	parameter_.stopperLandShake.Load();
	currentHp_ = parameter_.hp;

	if (WorldTransform* transform = GetTransform()) {
		position_ = transform->GetTranslate();
	}

	// 最初の行動をセットする
	behaviorController_.Init(*this);
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  更新
///////////////////////////////////////////////////////////////////////////////////////////////

void Boss::Update(const Math::Matrix4x4& viewProjection) {

	// 攻撃側からカメラ範囲を判定できるように覚えておく
	viewProjection_ = viewProjection;

	if (!IsValid()) {
		return;
	}

	// スクリーン座標上の固定位置に見えるワールド座標を求める
	const ScreenWorldPlaneAnchor::Params anchorParams{ parameter_.screenPos, parameter_.worldZ };
	position_ = screenAnchor_.Solve(viewProjection, anchorParams);

	if (WorldTransform* transform = GetTransform()) {
		transform->SetTranslate(position_);
	}

	// 基準位置を反映した後に行動を進める
	behaviorController_.Update(*this, AOENGINE::GameTimer::DeltaTime());
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  ダメージ
///////////////////////////////////////////////////////////////////////////////////////////////

void Boss::Damage(float amount) {
	currentHp_ -= amount;
	if (currentHp_ < 0.0f) {
		currentHp_ = 0.0f;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  今のフェーズ
///////////////////////////////////////////////////////////////////////////////////////////////

int32_t Boss::GetPhaseIndex() const {

	// 最大HPが未設定なら割合が出せないので、最終フェーズ扱いにしておく
	if (parameter_.hp <= 0.0f) {
		return static_cast<int32_t>(BossParameter::kPhaseSwitchCount);
	}

	const float ratio = currentHp_ / parameter_.hp;

	// 割合は高い順に並んでいる前提で、下回ったものの中で一番進んだフェーズを採用する
	int32_t phase = 0;
	for (std::size_t i = 0; i < BossParameter::kPhaseSwitchCount; ++i) {
		if (ratio <= parameter_.phaseSwitchRatio[i]) {
			phase = static_cast<int32_t>(i) + 1;
		}
	}

	return phase;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  デバッグ表示
///////////////////////////////////////////////////////////////////////////////////////////////

void Boss::Debug_Gui() {
	const char* bodyState = "null";
	if (IsValid()) {
		bodyState = "resolved";
	}

	ImGui::Text("body: %s", bodyState);
	ImGui::Text("hp: %.1f / %.1f  (phase %d)", currentHp_, parameter_.hp, GetPhaseIndex());
	ImGui::DragFloat3("world position", &position_.x, 0.1f);

	// ボスの状態を可視化 + 行動の強制切り替え
	behaviorController_.Debug_Gui(*this);

	// イージングは番号を覚えなくていいように、名前で選べる形で出す
	ImGui::SeparatorText("Easing");
	Math::SelectEasing(parameter_.fireballFallEaseKind, "FireballFall");
	Math::SelectEasing(parameter_.stopperEaseKind, "StopperFall");

	// 足止めが着地した時のカメラシェイク
	ImGui::SeparatorText("Attack3: Stopper Land Shake");
	ImGui::PushID("StopperLandShake");
	parameter_.stopperLandShake.Debug_Gui();
	parameter_.stopperLandShake.SaveAndLoad();
	if (ImGui::Button("Test Play")) {
		ShakeCamera(parameter_.stopperLandShake);
	}
	ImGui::PopID();
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  画面の上端より下に来ているか
///////////////////////////////////////////////////////////////////////////////////////////////

bool Boss::IsBelowCameraTop(const Math::Vector3& worldPosition) const {

	// ワールド座標をNDCへ変換する
	const Math::Vector3 ndc = TransformCoord(worldPosition, viewProjection_);

	// カメラの手前/奥に外れている場合は判定しない
	if (ndc.z < 0.0f || ndc.z > 1.0f) {
		return false;
	}

	// 上端を通過したら画面に入ってきたとみなす
	return ndc.y <= 1.0f;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  カメラに映っているか
///////////////////////////////////////////////////////////////////////////////////////////////

bool Boss::IsInCameraView(const Math::Vector3& worldPosition) const {

	// ワールド座標をNDCへ変換して範囲内か見る
	const Math::Vector3 ndc = TransformCoord(worldPosition, viewProjection_);

	return ndc.x >= -1.0f && ndc.x <= 1.0f &&
		ndc.y >= -1.0f && ndc.y <= 1.0f &&
		ndc.z >= 0.0f && ndc.z <= 1.0f;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  カメラを揺らす
///////////////////////////////////////////////////////////////////////////////////////////////

void Boss::ShakeCamera(const CameraShakeRequest& request) {
	if (pCamera_ == nullptr) {
		return;
	}
	// 揺れ方の中身はリクエスト側に任せ、ここは再生を頼むだけ
	pCamera_->PlayShake(request);
}

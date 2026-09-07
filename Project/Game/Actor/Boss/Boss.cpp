#include "Boss.h"

#include <algorithm>

#include "Engine/Module/Components/Animation/Animator.h"
#include "Engine/Module/Components/Collider/BoxCollider.h"
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
	parameter_.phaseChangeShake.Load();
	parameter_.damageShake.Load();
	parameter_.introLandShake.Load();
	currentHp_ = parameter_.hp;

	if (WorldTransform* transform = GetTransform()) {
		position_ = transform->GetTranslate();
	}

	scaleMultiplier_ = CVector3::UNIT;

	isInvincible_ = false;
	isDefeatFinished_ = false;
	isIntroDescending_ = false;
	introDescendTimer_ = 0.0f;
	introDescendOffsetY_ = 0.0f;
	SetRendering(true);

	animation_.Init();
	// 元の色を覚えさせる。被弾の演出が終わったらここへ戻る
	damageEffect_.Init(GetGameObject());

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

	// 被弾の色と揺れを進める
	damageEffect_.Update(GameTimer::DeltaTime(), GetGameObject(), MakeDamageEffectParams());

	// 攻撃側が見るのは基準位置のままにして、見た目だけ揺れのぶんずらす
	if (WorldTransform* transform = GetTransform()) {
		transform->SetTranslate(position_ + damageEffect_.GetPositionOffset());
	}

	// 見た目の大きさを反映する
	UpdateScale();

	// 基準位置を反映した後に行動を進める
	behaviorController_.Update(*this, AOENGINE::GameTimer::DeltaTime());

	// 行動が決まった後にアニメーションを合わせる
	UpdateAnimation();
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  待機中の更新
///////////////////////////////////////////////////////////////////////////////////////////////

void Boss::UpdateStandby(const Math::Matrix4x4& viewProjection) {

	viewProjection_ = viewProjection;

	if (!IsValid()) {
		return;
	}

	// 画面に対する位置決めだけは通常時と同じにしておく。
	// 通さないとSceneに置かれたままの座標で映ってしまう
	const ScreenWorldPlaneAnchor::Params anchorParams{ parameter_.screenPos, parameter_.worldZ };
	position_ = screenAnchor_.Solve(viewProjection, anchorParams);

	// 登場の降下を進める
	UpdateIntroDescend(GameTimer::DeltaTime());

	// 攻撃側が見るのは定位置のままにして、見た目だけ降りてくる途中に置く
	if (WorldTransform* transform = GetTransform()) {
		transform->SetTranslate(position_ + Math::Vector3(0.0f, introDescendOffsetY_, 0.0f));
	}

	UpdateScale();

	// 行動は進めないが、止まって見えないように待機だけ流しておく
	UpdateAnimation(kStandbyAnimationName);
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  登場の降下
///////////////////////////////////////////////////////////////////////////////////////////////

void Boss::StartIntroDescend() {

	isIntroDescending_ = true;
	introDescendTimer_ = 0.0f;
	introDescendOffsetY_ = parameter_.introDescendHeight;
}

void Boss::UpdateIntroDescend(float deltaTime) {

	if (!isIntroDescending_) {
		introDescendOffsetY_ = 0.0f;
		return;
	}

	introDescendTimer_ += deltaTime;

	// 時間が0なら一瞬で降ろす
	float ratio = 1.0f;
	if (parameter_.introDescendTime > 0.0f) {
		ratio = std::clamp(introDescendTimer_ / parameter_.introDescendTime, 0.0f, 1.0f);
	}

	// 高いところから定位置へ。だんだん速くなるイージングを想定している
	const float eased = Math::CallEasing(parameter_.introDescendEaseKind, ratio);
	introDescendOffsetY_ = parameter_.introDescendHeight * (1.0f - eased);

	if (ratio < 1.0f) {
		return;
	}

	// 着いたので揺らして終わり
	introDescendOffsetY_ = 0.0f;
	isIntroDescending_ = false;
	ShakeCamera(parameter_.introLandShake);
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  スケール
///////////////////////////////////////////////////////////////////////////////////////////////

void Boss::UpdateScale() {

	WorldTransform* transform = GetTransform();
	if (transform == nullptr) {
		return;
	}

	// 基準の大きさに演出用の倍率を掛けたものが、実際の見た目の大きさになる
	const Math::Vector3 scale = parameter_.baseScale * scaleMultiplier_;
	transform->SetScale(scale);

	// 見た目を大きくしても当たり判定が一緒に膨らまないように割り戻しておく
	BoxCollider* box = dynamic_cast<BoxCollider*>(GetCollider(kColliderTag));
	if (box == nullptr) {
		return;
	}

	Math::Vector3 size = parameter_.hitSize;
	if (scale.x != 0.0f) { size.x /= scale.x; }
	if (scale.y != 0.0f) { size.y /= scale.y; }
	if (scale.z != 0.0f) { size.z /= scale.z; }
	box->SetSize(size);
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  アニメーション
///////////////////////////////////////////////////////////////////////////////////////////////

void Boss::UpdateAnimation() {
	UpdateAnimation(behaviorController_.GetCurrentAnimationName());
}

void Boss::UpdateAnimation(const std::string& animationName) {

	BossAnimation::Context context{};
	if (BaseGameObject* body = GetGameObject()) {
		context.animator = body->GetAnimator();
	}
	context.behaviorName = &animationName;

	const BossAnimation::Params params{
		parameter_.animationBlendSpeed,
		parameter_.animationSpeed,
	};

	animation_.Update(context, params);
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  ダメージ
///////////////////////////////////////////////////////////////////////////////////////////////

void Boss::Damage(float amount) {

	// フェーズ切り替えの演出中などは受け付けない
	if (isInvincible_) {
		return;
	}

	// 被弾は行動に割り込んで1回だけ流す
	animation_.PlayDamage();

	// 赤くして、その場で小さく揺らす
	damageEffect_.Play(MakeDamageEffectParams());

	// se
	Engine::GetSoundManager()->Play("BossDamaged");

	currentHp_ -= amount;
	if (currentHp_ < 0.0f) {
		currentHp_ = 0.0f;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  被弾の演出へ渡す調整値
///////////////////////////////////////////////////////////////////////////////////////////////

BossDamageEffect::Params Boss::MakeDamageEffectParams() const {

	BossDamageEffect::Params params{};
	params.duration = parameter_.damageEffectTime;
	params.color = parameter_.damageColor;
	params.shake = parameter_.damageShake;

	return params;
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
	// HPは直接いじれるようにしておく。フェーズ切り替えと撃破をすぐ確認できる
	ImGui::DragFloat("current hp", &currentHp_, 0.1f, 0.0f, parameter_.hp);
	ImGui::SameLine();
	if (ImGui::Button("Kill")) {
		currentHp_ = 0.0f;
	}
	ImGui::SameLine();
	if (ImGui::Button("Full")) {
		currentHp_ = parameter_.hp;
	}

	ImGui::Text("hp: %.1f / %.1f  (phase %d)", currentHp_, parameter_.hp, GetPhaseIndex());

	const char* invincibleState = "false";
	if (isInvincible_) {
		invincibleState = "true";
	}
	ImGui::Text("invincible: %s", invincibleState);
	ImGui::DragFloat3("world position", &position_.x, 0.1f);

	// 行動が指定している名前と、実際に流れている名前の両方を出す
	ImGui::Text("animation: %s", behaviorController_.GetCurrentAnimationName().c_str());
	if (BaseGameObject* body = GetGameObject()) {
		if (Animator* animator = body->GetAnimator()) {
			ImGui::Text("  playing: %s  time %.2f / %.2f",
				animator->GetAnimationName().c_str(),
				animator->GetAnimationTime(),
				animator->GetAnimationDuration());
		}
	}

	// ボスの状態を可視化 + 行動の強制切り替え
	behaviorController_.Debug_Gui(*this);

	// イージングは番号を覚えなくていいように、名前で選べる形で出す
	ImGui::SeparatorText("Easing");
	Math::SelectEasing(parameter_.fireballFallEaseKind, "FireballFall");
	Math::SelectEasing(parameter_.stopperEaseKind, "StopperFall");
	Math::SelectEasing(parameter_.introDescendEaseKind, "IntroDescend");

	// 足止めが着地した時のカメラシェイク
	ImGui::SeparatorText("Attack3: Stopper Land Shake");
	ImGui::PushID("StopperLandShake");
	parameter_.stopperLandShake.Debug_Gui();
	parameter_.stopperLandShake.SaveAndLoad();
	if (ImGui::Button("Test Play")) {
		ShakeCamera(parameter_.stopperLandShake);
	}
	ImGui::PopID();

	// 登場で着いた時の揺れ
	ImGui::SeparatorText("Intro Land Shake");
	ImGui::PushID("IntroLandShake");
	parameter_.introLandShake.Debug_Gui();
	parameter_.introLandShake.SaveAndLoad();
	if (ImGui::Button("Test Play")) {
		StartIntroDescend();
	}
	ImGui::PopID();

	// 被弾した時の、ボス自身の揺れ
	ImGui::SeparatorText("Damage Shake");
	ImGui::PushID("DamageShake");
	parameter_.damageShake.Debug_Gui();
	parameter_.damageShake.SaveAndLoad();
	if (ImGui::Button("Test Play")) {
		damageEffect_.Play(MakeDamageEffectParams());
	}
	ImGui::PopID();

	// フェーズが上がった時のカメラシェイク
	ImGui::SeparatorText("Phase Change Shake");
	ImGui::PushID("PhaseChangeShake");
	parameter_.phaseChangeShake.Debug_Gui();
	parameter_.phaseChangeShake.SaveAndLoad();
	if (ImGui::Button("Test Play")) {
		ShakeCamera(parameter_.phaseChangeShake);
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
//  画面の下端より下へ抜けたか
///////////////////////////////////////////////////////////////////////////////////////////////

bool Boss::IsBelowCameraBottom(const Math::Vector3& worldPosition) const {

	// ワールド座標をNDCへ変換する
	const Math::Vector3 ndc = TransformCoord(worldPosition, viewProjection_);

	// カメラの手前/奥に外れている場合は判定しない
	if (ndc.z < 0.0f || ndc.z > 1.0f) {
		return false;
	}

	// 下端を通り過ぎたら画面から抜けたとみなす
	return ndc.y < -1.0f;
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

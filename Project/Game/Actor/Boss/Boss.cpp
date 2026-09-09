#include "Boss.h"

#include <algorithm>
#include <cmath>
#include <numbers>

#include "Engine/Module/Components/Animation/Animator.h"
#include "Engine/Module/Components/Collider/BoxCollider.h"
#include "Engine/Module/Components/GameObject/BaseGameObject.h"
#include "Engine/Module/Components/WorldTransform.h"
#include "Engine/Lib/Math/Easing.h"
#include "Engine/Module/Components/Effect/BaseParticles.h"
#include "Engine/System/Manager/ImGuiManager.h"
#include "Engine/System/Manager/ParticleManager.h"
#include "Engine/Utilities/Logger.h"

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
	damagedCount_ = 0;
	hitStopTimer_ = 0.0f;
	isPhaseChanging_ = false;
	worldZOffset_ = 0.0f;
	dummySwayTimer_ = 0.0f;
	dummySwayRotate_ = 0.0f;
	GameTimer::SetTimeScale(1.0f);
	attackEffectRemaining_ = 0;
	attackEffectTimer_ = 0.0f;
	isAttackPulsing_ = false;
	attackPulseTimer_ = 0.0f;
	isIntroDescending_ = false;
	introDescendTimer_ = 0.0f;
	introDescendOffsetY_ = 0.0f;
	SetRendering(true);

	// 攻撃に入った合図のエフェクト。使い回すのでここで1つ作っておく
	if (attackEffect_ == nullptr) {
		attackEffect_ = ParticleManager::GetInstance()->CreateParticle(kAttackEffectName);
		if (attackEffect_ == nullptr) {
			Logger::CommentLog(kAttackEffectName + "が作れなかったため、攻撃のエフェクトは出しません");
		}
	}

	if (attackEffect_ != nullptr) {
		// 攻撃を出した瞬間に一度だけ吹くものなので、垂れ流しにはしない
		attackEffect_->SetLoop(false);
		// 作った直後は動いている。攻撃が始まるまで出したくないので止めておく
		attackEffect_->SetIsStop(true);
	}

	// フェーズが上がった時のエフェクト。作り方は攻撃側と同じ
	if (phaseChangeEffect_ == nullptr) {
		phaseChangeEffect_ = ParticleManager::GetInstance()->CreateParticle(kPhaseChangeEffectName);
		if (phaseChangeEffect_ == nullptr) {
			Logger::CommentLog(kPhaseChangeEffectName + "が作れなかったため、フェーズ切り替えのエフェクトは出しません");
		}
	}

	if (phaseChangeEffect_ != nullptr) {
		phaseChangeEffect_->SetLoop(false);
		phaseChangeEffect_->SetIsStop(true);
	}

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
	const ScreenWorldPlaneAnchor::Params anchorParams{
		parameter_.screenPos, parameter_.worldZ + worldZOffset_ };
	position_ = screenAnchor_.Solve(viewProjection, anchorParams);

	// 的として置いている間の、左右の揺れを進める
	UpdateDummySway(GameTimer::DeltaTime());

	// 被弾の色と揺れを進める
	damageEffect_.Update(GameTimer::DeltaTime(), GetGameObject(), MakeDamageEffectParams());

	// 攻撃側が見るのは基準位置のままにして、見た目だけ揺れのぶんずらす
	if (WorldTransform* transform = GetTransform()) {
		transform->SetTranslate(position_ + damageEffect_.GetPositionOffset() + CalcViewOffset());
	}
	ApplyDummyRotate();

	// 残っているエフェクトを間隔を空けて出す
	UpdateAttackEffect(GameTimer::DeltaTime());

	// 大きさの倍率を決めてから見た目へ反映する
	UpdateAttackPulse(GameTimer::DeltaTime());
	UpdateScale();

	// 基準位置を反映した後に行動を進める
	behaviorController_.Update(*this, AOENGINE::GameTimer::DeltaTime());

	// 行動が決まった後にアニメーションを合わせる
	UpdateAnimation();
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  待機中の更新
///////////////////////////////////////////////////////////////////////////////////////////////

void Boss::UpdateStandby(const Math::Matrix4x4& viewProjection, float deltaTime) {

	viewProjection_ = viewProjection;

	if (!IsValid()) {
		return;
	}

	// 画面に対する位置決めだけは通常時と同じにしておく。
	// 通さないとSceneに置かれたままの座標で映ってしまう
	const ScreenWorldPlaneAnchor::Params anchorParams{ parameter_.screenPos, parameter_.worldZ };
	position_ = screenAnchor_.Solve(viewProjection, anchorParams);

	// 登場の降下を進める。イントロの早送りに乗せたいので、時間は外から受け取る
	UpdateIntroDescend(deltaTime);

	// 攻撃側が見るのは定位置のままにして、見た目だけ降りてくる途中に置く
	if (WorldTransform* transform = GetTransform()) {
		transform->SetTranslate(
			position_ + Math::Vector3(0.0f, introDescendOffsetY_, 0.0f) + CalcViewOffset());
	}
	ApplyDummyRotate();

	UpdateScale();

	// 行動は進めないが、止まって見えないように待機だけ流しておく
	UpdateAnimation(kStandbyAnimationName);
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  攻撃を出した時のパルス
///////////////////////////////////////////////////////////////////////////////////////////////

void Boss::PlayAttackEffect() {

	if (attackEffect_ == nullptr) {
		return;
	}

	// 1発目はその場で出して、残りは間隔を空けて追いかけさせる
	attackEffectRemaining_ = (std::max)(parameter_.attackEffectCount, 1);
	attackEffectTimer_ = 0.0f;

	EmitAttackEffect();
}

void Boss::PlayPhaseChangeEffect() {

	if (phaseChangeEffect_ == nullptr) {
		return;
	}

	// 出す場所を決めてから動かす。Reset()が止まっている状態を解除する
	phaseChangeEffect_->SetPos(position_ + parameter_.attackEffectOffset);
	phaseChangeEffect_->Reset();
}

void Boss::UpdateAttackEffect(float deltaTime) {

	if (attackEffectRemaining_ <= 0) {
		return;
	}

	attackEffectTimer_ += deltaTime;
	if (attackEffectTimer_ < parameter_.attackEffectInterval) {
		return;
	}

	attackEffectTimer_ = 0.0f;
	EmitAttackEffect();
}

void Boss::EmitAttackEffect() {

	if (attackEffect_ == nullptr || attackEffectRemaining_ <= 0) {
		return;
	}

	--attackEffectRemaining_;

	// 先に出す場所を決めてから動かす
	attackEffect_->SetPos(position_ + parameter_.attackEffectOffset);
	attackEffect_->Reset();
}

void Boss::StartAttackPulse() {

	isAttackPulsing_ = true;
	attackPulseTimer_ = 0.0f;
}

void Boss::StopAttackPulse() {

	if (!isAttackPulsing_) {
		return;
	}

	isAttackPulsing_ = false;
	attackPulseTimer_ = 0.0f;
	scaleMultiplier_ = CVector3::UNIT;
}

void Boss::UpdateAttackPulse(float deltaTime) {

	// 動いていない間は倍率に触らない
	if (!isAttackPulsing_) {
		return;
	}

	attackPulseTimer_ += deltaTime;

	// 時間が0なら膨らませずに終わる
	float ratio = 1.0f;
	if (parameter_.attackPulseTime > 0.0f) {
		ratio = std::clamp(attackPulseTimer_ / parameter_.attackPulseTime, 0.0f, 1.0f);
	}

	// 指定回数ぶんに割ってから、イージングで歪ませて sin で 0->1->0 の山を作る
	const float local = CalcPulseLocalRatio(ratio);
	const float eased = Math::CallEasing(parameter_.attackPulseEaseKind, local);
	const float pulse = std::sin(eased * std::numbers::pi_v<float>);
	const float rate = 1.0f + pulse * parameter_.attackPulseScaleRate;

	scaleMultiplier_ = Math::Vector3(rate, rate, rate);

	if (ratio >= 1.0f) {
		isAttackPulsing_ = false;
		scaleMultiplier_ = CVector3::UNIT;
	}
}

float Boss::CalcPulseLocalRatio(float ratio) const {

	// 1回だけなら割る必要がない
	if (parameter_.attackPulseCount <= 1) {
		return ratio;
	}

	// 最後は必ず山の終わりにしたいので、1.0はそのまま返す
	if (ratio >= 1.0f) {
		return 1.0f;
	}

	const float scaled = ratio * static_cast<float>(parameter_.attackPulseCount);
	return scaled - std::floor(scaled);
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
//  ヒットストップ
///////////////////////////////////////////////////////////////////////////////////////////////

void Boss::UpdateHitStop() {

	if (hitStopTimer_ <= 0.0f) {
		return;
	}

	// 止めている間はDeltaTimeが0になるので、時間の進みは固定値の方で数える
	hitStopTimer_ -= GameTimer::FixedDeltaTime();
	if (hitStopTimer_ > 0.0f) {
		return;
	}

	hitStopTimer_ = 0.0f;
	GameTimer::SetTimeScale(1.0f);
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  見た目だけのずらし
///////////////////////////////////////////////////////////////////////////////////////////////

Math::Vector3 Boss::CalcViewOffset() const {

	if (!isTrainingDummy_) {
		return CVector3::ZERO;
	}

	// ずらしはモデルの大きさで持っているので、実際の大きさへ直してから足す。
	// こうしておくとDummy Scaleを変えても足元の位置がずれない
	return parameter_.dummyOffset * parameter_.dummyScale;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  的の揺れ
///////////////////////////////////////////////////////////////////////////////////////////////

void Boss::UpdateDummySway(float deltaTime) {

	if (!isTrainingDummy_) {
		dummySwayTimer_ = 0.0f;
		dummySwayRotate_ = 0.0f;
		return;
	}

	const float duration = parameter_.dummySwayTime;
	if (duration <= 0.0f) {
		dummySwayRotate_ = 0.0f;
		return;
	}

	// 往復1周ぶん進んだら頭へ戻す
	dummySwayTimer_ += deltaTime;
	while (dummySwayTimer_ >= duration) {
		dummySwayTimer_ -= duration;
	}

	// 前半で右へ、後半で左へ。折り返しは同じ進み具合を逆に辿る
	float ratio = dummySwayTimer_ / duration * 2.0f;
	if (ratio > 1.0f) {
		ratio = 2.0f - ratio;
	}

	// 0で左へ倒れきり、1で右へ倒れきる
	const float eased = Math::CallEasing(parameter_.dummySwayEaseKind, ratio);
	const float degree = (eased * 2.0f - 1.0f) * parameter_.dummySwayAngle;

	dummySwayRotate_ = degree * kToRadian;
}

void Boss::ApplyDummyRotate() const {

	// 的でない時に触ると、シーンに置かれている向きを壊してしまう
	if (!isTrainingDummy_) {
		return;
	}

	WorldTransform* transform = GetTransform();
	if (transform == nullptr) {
		return;
	}

	// 奥行きを軸に倒すので、画面の中では左右に傾いて見える
	transform->SetRotate(Math::Quaternion::AngleAxis(dummySwayRotate_, CVector3::FORWARD));
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  スケール
///////////////////////////////////////////////////////////////////////////////////////////////

void Boss::UpdateScale() {

	WorldTransform* transform = GetTransform();
	if (transform == nullptr) {
		return;
	}

	// 的として置いている間は別のモデルなので、基準の大きさもそれ用のものを使う
	Math::Vector3 baseScale = parameter_.baseScale;
	if (isTrainingDummy_) {
		baseScale = parameter_.dummyScale;
	}

	// 基準の大きさに演出用の倍率を掛けたものが、実際の見た目の大きさになる
	const Math::Vector3 scale = baseScale * scaleMultiplier_;
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

	++damagedCount_;

	// 当たった手応えを出すために、ほんの少しだけ時間を止める
	if (parameter_.hitStopTime > 0.0f) {
		hitStopTimer_ = parameter_.hitStopTime;
		GameTimer::SetTimeScale(parameter_.hitStopTimeScale);
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
	Math::SelectEasing(parameter_.attackPulseEaseKind, "AttackPulse");
	Math::SelectEasing(parameter_.phaseChangeZoomEaseKind, "PhaseChangeZoomIn");
	Math::SelectEasing(parameter_.phaseChangeReturnEaseKind, "PhaseChangeZoomOut");
	Math::SelectEasing(parameter_.dummySwayEaseKind, "DummySway");
	Math::SelectEasing(parameter_.phaseChangeBlurEaseKind, "PhaseChangeBlur");

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

void Boss::SetCameraExtraOffset(const Math::Vector3& offset) {
	if (pCamera_ == nullptr) {
		return;
	}
	pCamera_->SetExtraOffset(offset);
}

Math::Vector3 Boss::GetCameraWorldPosition() const {
	if (pCamera_ == nullptr) {
		return CVector3::ZERO;
	}
	return pCamera_->GetWorldPosition();
}

#include "BossAttackBeam.h"

#include "Engine/Module/Components/Collider/BaseCollider.h"
#include "Engine/Module/Components/Collider/BoxCollider.h"
#include "Engine/Module/Components/GameObject/BaseGameObject.h"
#include "Engine/Module/Components/WorldTransform.h"
#include "Engine/System/Manager/PrefabManager.h"
#include "Engine/System/Manager/ParticleEffectManager.h"
#include "Engine/Utilities/SceneObjectFinder.h"
#include "Engine/Utilities/Logger.h"

#include "Game/Actor/Boss/Boss.h"

namespace {
	/// <summary>Prefabから BaseGameObject を生成する。生成できなければ nullptr</summary>
	AOENGINE::BaseGameObject* InstantiateObject(const std::string& prefabName) {
		AOENGINE::SceneObject* root = AOENGINE::PrefabManager::GetInstance()->Instantiate(prefabName);
		return dynamic_cast<AOENGINE::BaseGameObject*>(root);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  状態ごとの更新処理を用意する
///////////////////////////////////////////////////////////////////////////////////////////////

BossAttackBeam::BossAttackBeam() {

	// 状態ごとの処理を登録する
	phaseUpdaters_[ToIndex(Phase::Warning)] = [this](Boss& boss, float deltaTime) { UpdateWarningPhase(boss, deltaTime); };
	phaseUpdaters_[ToIndex(Phase::Windup)] = [this](Boss& boss, float deltaTime) { UpdateWindupPhase(boss, deltaTime); };
	phaseUpdaters_[ToIndex(Phase::Beam)] = [this](Boss& boss, float deltaTime) { UpdateBeamPhase(boss, deltaTime); };

	chargeParticle_ = AOENGINE::ParticleManager::GetInstance()->CreateParticle("BossBeamCharge");
	if (!chargeParticle_) {
		AOENGINE::Logger::AssertLog("BossBeamChargeが生成できませんでした");
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  予測線と危険マークを点滅させている状態
///////////////////////////////////////////////////////////////////////////////////////////////

void BossAttackBeam::UpdateWarningPhase(Boss& boss, float deltaTime) {

	const BossParameter& param = boss.GetParameter();

	// アニメーションと予測線は、どちらも1回ぶんの始まりからの経過時間で出す。
	// 待ちの大小を入れ替えれば、構えてから警告にも、警告してから構えにもできる
	if (!hasStartedAnimation_ && phaseTimer_ >= param.beamAnimationDelay) {
		hasStartedAnimation_ = true;
	}

	if (!hasSpawnedWarning_ && phaseTimer_ >= param.beamWarningDelay) {
		SpawnWarning(boss);
		warningSound_ = Engine::GetSoundManager()->Play("BeamAim");
		hasSpawnedWarning_ = true;
	}

	UpdateWarning(boss, deltaTime);

	// 予測線を見せる時間は、それが出てからで数える
	if (phaseTimer_ < param.beamWarningDelay + param.beamWarningTime) {
		return;
	}

	// ビームを出すまでの待ちを数え直す
	ResetStartDelay();
	ChangePhase(Phase::Windup);
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  ビームのアニメーションを流して構えている状態
///////////////////////////////////////////////////////////////////////////////////////////////

void BossAttackBeam::UpdateWindupPhase(Boss& boss, float deltaTime) {

	// 構えている間も予測線は点滅させ続ける
	UpdateWarning(boss, deltaTime);

	// アニメーションを見せてからビームを出す
	if (WaitStartDelay(deltaTime, boss.GetParameter().beamStartDelay)) {
		Engine::GetSoundManager()->Stop(warningSound_);
		return;
	}

	DestroyWarning();
	SpawnBeam(boss);
	ChangePhase(Phase::Beam);
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  ビームが左から通過している状態
///////////////////////////////////////////////////////////////////////////////////////////////

void BossAttackBeam::UpdateBeamPhase(Boss& boss, float deltaTime) {

	UpdateBeam(boss, deltaTime);

	// ビームの時間が終わったら次の回へ
	if (phaseTimer_ < boss.GetParameter().beamTime) {
		return;
	}

	DestroyBeam();
	++firedCount_;

	// 規定回数撃ったら終わり
	if (firedCount_ >= boss.GetParameter().beamCount) {
		isFinished_ = true;
		phaseTimer_ = 0.0f;
		return;
	}

	// まだ残っていれば次の回へ。予測線もアニメーションもWarningの更新が出し直す
	ChangePhase(Phase::Warning);
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  開始
///////////////////////////////////////////////////////////////////////////////////////////////

void BossAttackBeam::Enter(Boss& boss) {

	(void)boss;

	// 初期化。アニメーションも予測線も、待ちが明けてからWarningの更新が出す
	firedCount_ = 0;
	isFinished_ = false;
	ChangePhase(Phase::Warning);
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  終了
///////////////////////////////////////////////////////////////////////////////////////////////

void BossAttackBeam::Exit(Boss& boss) {
	(void)boss;

	// 途中で行動が切り替わった場合、出しっぱなしのものを片付ける
	DestroyWarning();
	DestroyBeam();
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  更新
///////////////////////////////////////////////////////////////////////////////////////////////

void BossAttackBeam::Update(Boss& boss, float deltaTime) {

	phaseTimer_ += deltaTime;

	// 現在の状態の処理を呼ぶ
	phaseUpdaters_[ToIndex(phase_)](boss, deltaTime);
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  状態の切り替え
///////////////////////////////////////////////////////////////////////////////////////////////

void BossAttackBeam::ChangePhase(Phase next) {
	phase_ = next;
	phaseTimer_ = 0.0f;

	// 1回ぶんの始まり。アニメーションと予測線を出し直す
	if (next == Phase::Warning) {
		hasStartedAnimation_ = false;
		hasSpawnedWarning_ = false;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  予測線と危険マークの生成・破棄
///////////////////////////////////////////////////////////////////////////////////////////////

void BossAttackBeam::SpawnWarning(const Boss& boss) {

	const BossParameter& param = boss.GetParameter();

	// 予測線を出す瞬間のプレイヤーの高さを覚える
	if (AOENGINE::BaseGameObject* player = FindSceneObject<AOENGINE::BaseGameObject>("Player")) {
		if (AOENGINE::WorldTransform* playerTransform = player->GetTransform()) {
			beamPosY_ = playerTransform->GetTranslate().y;
		}
	}

	// 点滅を表示状態から始める
	blinkTimer_ = 0.0f;
	isBlinkVisible_ = true;

	// 予測線(大きさは調整項目、Xとzはprefabの設定をそのまま使う)
	if (AOENGINE::BaseGameObject* object = InstantiateObject(kWarningLinePrefab_)) {
		warningLine_.Bind(object);
		if (AOENGINE::WorldTransform* transform = warningLine_.GetTransform()) {
			Math::Vector3 pos = transform->GetTranslate();
			pos.y = beamPosY_;
			transform->SetTranslate(pos);
			transform->SetScale(param.beamWarningLineSize);
		}
	}

	// 三角の危険マーク
	if (AOENGINE::BaseGameObject* object = InstantiateObject(kWarningMarkPrefab_)) {
		warningMark_.Bind(object);
		if (AOENGINE::WorldTransform* transform = warningMark_.GetTransform()) {
			Math::Vector3 pos = transform->GetTranslate();
			pos.y = beamPosY_;
			transform->SetTranslate(pos);
			transform->SetScale(param.beamWarningMarkSize);
		}
	}

	// チャージパーティクルを出す
	if (chargeParticle_) {
		chargeParticle_->Reset();
		chargeParticle_->SetPos(Math::Vector3(param.beamChargePosX, beamPosY_, 0.0f));
	} else {
		AOENGINE::Logger::AssertLog("BossBeamChargeが生成出来ませんでした");
	}

}

void BossAttackBeam::DestroyWarning() {

	// ビーム中は点滅を消すので、ここで両方とも破棄する
	if (warningLine_.IsValid()) {
		warningLine_.Destroy();
	}
	if (warningMark_.IsValid()) {
		warningMark_.Destroy();
	}
	warningLine_.Unbind();
	warningMark_.Unbind();

	if (chargeParticle_) {
		chargeParticle_->SetIsStop(true);
	}
}

void BossAttackBeam::UpdateWarning(const Boss& boss, float deltaTime) {

	blinkTimer_ += deltaTime;
	if (blinkTimer_ < boss.GetParameter().beamBlinkInterval) {
		return;
	}
	blinkTimer_ = 0.0f;

	// 表示/非表示を切り替えて点滅させる
	isBlinkVisible_ = !isBlinkVisible_;
	if (warningLine_.IsValid()) {
		warningLine_.SetRendering(isBlinkVisible_);
	}
	if (warningMark_.IsValid()) {
		warningMark_.SetRendering(isBlinkVisible_);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  ビームの生成・破棄・移動
///////////////////////////////////////////////////////////////////////////////////////////////

void BossAttackBeam::SpawnBeam(const Boss& boss) {

	AOENGINE::BaseGameObject* object = InstantiateObject(kBeamName_);
	if (!object) {
		return;
	}

	beam_.Bind(object);

	const BossParameter& param = boss.GetParameter();

	// 画面外から、覚えておいた高さに出す
	if (AOENGINE::WorldTransform* transform = beam_.GetTransform()) {
		Math::Vector3 pos = transform->GetTranslate();
		pos.x = param.beamStartX;
		pos.y = beamPosY_;
		transform->SetTranslate(pos);
		transform->SetScale(param.beamSize);
	}

	// エフェクトを出す
	AOENGINE::ParticleEffectManager::GetInstance()->Play(
		"laserFireEffect", Math::Vector3(param.beamEffectPosX, beamPosY_, 0.0f));

	// サウンドを鳴らす
	shotSound_ = Engine::GetSoundManager()->Play("BeamShot");
}

void BossAttackBeam::DestroyBeam() {

	if (beam_.IsValid()) {
		beam_.Destroy();
	}
	beam_.Unbind();
}

void BossAttackBeam::UpdateBeam(const Boss& boss, float deltaTime) {

	if (!beam_.IsValid()) {
		return;
	}

	// 左から右へ動かす
	if (AOENGINE::WorldTransform* transform = beam_.GetTransform()) {
		Math::Vector3 pos = transform->GetTranslate();
		pos.x += boss.GetParameter().beamSpeed * deltaTime;
		transform->SetTranslate(pos);
	}
}

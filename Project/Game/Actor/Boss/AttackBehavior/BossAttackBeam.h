#pragma once

#include <array>
#include <functional>

#include "Game/Actor/Boss/AttackBehavior/BaseBossAttackBehavior.h"
#include "Engine/Module/Components/GameObject/BaseEntity.h"

class BossAttackBeam :public BaseBossAttackBehavior {
public:
	BossAttackBeam();
	~BossAttackBeam()override = default;

	// 入り、更新、抜け
	void Enter(Boss& boss)override;
	void Update(Boss& boss, float deltaTime)override;
	void Exit(Boss& boss) override;

private:
	// ビーム1回ぶんの進行状態
	enum class Phase {
		Warning,	// 予測線と危険マークを点滅させている
		Windup,		// ビームのアニメーションを流して構えている
		Beam,		// ビームが左から通過している
	};

	// 状態の数
	static constexpr std::size_t kPhaseCount = 3;

private:

	std::size_t ToIndex(Phase phase) const { return static_cast<std::size_t>(phase); }

	/// <summary>
	/// 状態を切り替える
	/// </summary>
	void ChangePhase(Phase next);

	/// <summary>
	/// 予測線を点滅させ、時間が来たらビームへ移る
	/// </summary>
	void UpdateWarningPhase(Boss& boss, float deltaTime);

	/// <summary>
	/// ビームのアニメーションを流し、待ちが明けたらビームを出す
	/// </summary>
	void UpdateWindupPhase(Boss& boss, float deltaTime);

	/// <summary>
	/// ビームを動かし、時間が来たら次の回へ移る
	/// </summary>
	void UpdateBeamPhase(Boss& boss, float deltaTime);

	/// <summary>
	/// 予測線と危険マークを出す
	/// </summary>
	void SpawnWarning(const Boss& boss);

	/// <summary>
	/// 予測線と危険マークを消す
	/// </summary>
	void DestroyWarning();

	/// <summary>
	/// ビームを画面外に出す
	/// </summary>
	void SpawnBeam(const Boss& boss);

	/// <summary>
	/// ビームを消す
	/// </summary>
	void DestroyBeam();

	/// <summary>
	/// 予測線と危険マークの点滅
	/// </summary>
	void UpdateWarning(const Boss& boss, float deltaTime);

	/// <summary>
	/// ビームを左から右へ動かす
	/// </summary>
	void UpdateBeam(const Boss& boss, float deltaTime);

private:

	// 調整項目は BossParameter 側にある

	// 内部状態
	Phase phase_ = Phase::Warning;
	int firedCount_ = 0;		// 撃った回数
	float phaseTimer_ = 0.0f;	// 現在のフェーズの経過時間
	float blinkTimer_ = 0.0f;	// 点滅用タイマー
	bool isBlinkVisible_ = true;// 点滅の表示状態
	bool isFinished_ = false;	// 終わったか

	// 予測線を出した時のプレイヤーの高さ
	float beamPosY_ = 0.0f;

	// 状態ごとの更新処理
	std::array<std::function<void(Boss& boss, float deltaTime)>, kPhaseCount> phaseUpdaters_;

	// 生成したオブジェクト
	AOENGINE::BaseEntity warningLine_;	// 予測線
	AOENGINE::BaseEntity warningMark_;	// 三角の危険マーク
	AOENGINE::BaseEntity beam_;			// ビーム本体

	// Prefab名
	const std::string kWarningLinePrefab_ = "BeamWarningLine";
	const std::string kWarningMarkPrefab_ = "BeamWarningMark";
	const std::string kBeamName_ = "Beam";

public:// acceccer

	bool IsFinished() const override { return isFinished_; }
	const std::string& GetName() const override {
		static const std::string kName = "BossAttackBeam";
		return kName;
	}

	const std::string& GetAnimationName() const override {
		static const std::string kAttackName = "attack2";
		static const std::string kIdleName = "idle";

		// 予測線を見せている間はまだ構えない。ビームの直前から流す
		if (phase_ == Phase::Warning) {
			return kIdleName;
		}

		return kAttackName;
	}
};

#pragma once

#include <array>
#include <cstddef>
#include <functional>

#include "Engine/Lib/Math/Vector3.h"
#include "Game/Actor/Boss/AttackBehavior/BaseBossAttackBehavior.h"

/// <summary>
/// HPが減ってフェーズが上がった時に割り込む行動
/// </summary>
class BossPhaseChange : public BaseBossAttackBehavior {
public:

	BossPhaseChange();
	~BossPhaseChange() override = default;

	// 入り、更新、抜け
	void Enter(Boss& boss) override;
	void Update(Boss& boss, float deltaTime) override;
	void Exit(Boss& boss) override;

private:

	// 演出の流れ
	enum class Step {
		StartWait,	// 止めてからカメラを動かすまでの間
		ZoomIn,		// カメラを寄せる
		Hold,		// 寄せたまま見せる
		ZoomOut,	// カメラを戻す
		EndWait,	// 戻しきってから再開するまでの間
		Done,		// 終わった
	};
	// 流れの数
	static constexpr std::size_t kStepCount = 6;

	std::size_t ToIndex(Step step) const { return static_cast<std::size_t>(step); }

	/// <summary>次の流れへ進める</summary>
	void ChangeStep(Step next);

	/// <summary>今の流れの長さを調整値から引く</summary>
	float CalcStepDuration(const Boss& boss) const;

	/// <summary>
	/// カメラのずらしと、ボスの奥行きをまとめて入れる。
	/// </summary>
	void Apply(Boss& boss, float ratio) const;

	/// <summary>
	/// 演出中のボスの奥行きのずらしを求める。
	/// </summary>
	float CalcWorldZOffset(const Boss& boss, float ratio) const;

	// 流れごとのカメラの動かし方。stepUpdaters_ から呼ばれる
	void UpdateZoomIn(Boss& boss, float ratio) const;
	void UpdateZoomOut(Boss& boss, float ratio) const;
	void UpdateHold(Boss& boss) const;
	void UpdateWait(Boss& boss) const;

	/// <summary>膨らんで戻る動きを見た目へ反映する</summary>
	void UpdateScalePulse(Boss& boss, float ratio) const;

private:

	// 流れごとの進め方。引数は0〜1の進み具合
	using StepUpdater = std::function<void(Boss&, float)>;
	std::array<StepUpdater, kStepCount> stepUpdaters_;

	// 今の流れと、その経過時間
	Step step_ = Step::StartWait;
	float stepTimer_ = 0.0f;

	// 演出を始めた時のカメラの奥行き。寄せる前を基準にしたいので覚えておく
	float cameraBaseZ_ = 0.0f;
	bool hasCamera_ = false;

	// 始まってからの経過時間。膨らむ動きに使う
	float elapsedTime_ = 0.0f;

	bool isFinished_ = false;			// 終わったか
	bool hasStartedAnimation_ = false;	// 専用のアニメーションへ切り替えたか
	bool hasPlayedEffect_ = false;		// エフェクトを出したか

public:// acceccer

	bool IsFinished() const override { return isFinished_; }

	// 攻撃ではないので、発動のパルスは出さない
	bool IsAttack() const override { return false; }

	const std::string& GetName() const override {
		static const std::string kName = "BossPhaseChange";
		return kName;
	}

	// フェーズ切り替え専用のクリップ。モデル側の綴りに合わせている。
	// 開始を遅らせている間は待機を流しておく
	const std::string& GetAnimationName() const override {
		static const std::string kIdleName = "idle";
		static const std::string kName = "faseChange";

		if (!hasStartedAnimation_) {
			return kIdleName;
		}
		return kName;
	}
};

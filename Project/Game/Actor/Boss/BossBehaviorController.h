#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>

#include "Game/Actor/Boss/AttackBehavior/BaseBossAttackBehavior.h"
#include "Game/Actor/Boss/BossParameter.h"

class Boss;

/// <summary>
/// ボスの攻撃行動を選んで切り替える管理クラス
/// </summary>
class BossBehaviorController {
public:
	BossBehaviorController() = default;
	~BossBehaviorController() = default;

	// 最初の行動をセットする
	void Init(Boss& boss);

	// 現在の行動を進め、終わったら次の行動へ切り替える
	void Update(Boss& boss, float deltaTime);

	// 行動を差し替える
	void ChangeBehavior(Boss& boss, std::unique_ptr<BaseBossAttackBehavior> next);

	// 現在の行動の表示と、確認用の強制切り替え
	void Debug_Gui(Boss& boss);

private:

	// 攻撃の種類。増やしたらkAttackKindCountも合わせる
	enum class AttackKind {
		FallFire,
		Beam,
		Stopper,
	};
	static constexpr std::size_t kAttackKindCount = 3;

	static std::size_t ToIndex(AttackKind kind) { return static_cast<std::size_t>(kind); }

	// 次に行う行動を決める。攻撃とIdleを交互に返す
	std::unique_ptr<BaseBossAttackBehavior> SelectNextBehavior(const Boss& boss);

	/// <summary>
	/// 今のフェーズで解禁されている攻撃のうち、直前と違うものの中から
	/// </summary>
	AttackKind SelectNextAttack(const Boss& boss) const;

	/// <summary>その攻撃が解禁されるフェーズ番号を引く</summary>
	static int32_t GetUnlockPhase(const BossParameter& parameter, AttackKind kind);

	/// <summary>種類から攻撃の行動クラスを作る</summary>
	std::unique_ptr<BaseBossAttackBehavior> CreateAttack(const Boss& boss, AttackKind kind);

private:

	// 現在の行動
	std::unique_ptr<BaseBossAttackBehavior> currentBehavior_;

	// 攻撃ごとの使用回数。少ないものを優先して選ぶために数えておく
	std::array<int32_t, kAttackKindCount> attackUseCounts_{};

	// 直前に行った攻撃。同じ攻撃を続けて出さないために覚えておく
	AttackKind previousAttack_ = AttackKind::FallFire;
	bool hasPreviousAttack_ = false;

	// 次はIdleを挟むか。攻撃 -> Idle -> 攻撃 と交互に繰り返す
	bool nextIsIdle_ = false;

	// 行動が無い時に返す名前
	const std::string kNoneName_ = "None";

public:// acceccer

	const std::string& GetCurrentName() const;
};

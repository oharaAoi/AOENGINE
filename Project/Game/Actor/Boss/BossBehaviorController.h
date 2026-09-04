#pragma once

#include <memory>
#include <string>

#include "Game/Actor/Boss/AttackBehavior/BaseBossAttackBehavior.h"

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

	// 次に行う行動を決める
	std::unique_ptr<BaseBossAttackBehavior> SelectNextBehavior(const Boss& boss);

private:

	// 現在の行動
	std::unique_ptr<BaseBossAttackBehavior> currentBehavior_;

	// 行動が無い時に返す名前
	const std::string kNoneName_ = "None";

public:// acceccer

	const std::string& GetCurrentName() const;
};

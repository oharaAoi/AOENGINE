#pragma once

#include "Game/Actor/Boss/AttackBehavior/BaseBossAttackBehavior.h"
#include "Engine/Module/Components/GameObject/BaseEntity.h"
#include <list>

class Block;
class StageBlockField;

/// <summary>
/// プレイヤーがいない足場へ足止めを落とす。
/// 落ちてきた足止めはブロックの上に乗り、しばらく残ってから消える。
/// </summary>
class BossAttackStopper :public BaseBossAttackBehavior {
public:
	BossAttackStopper() = default;
	~BossAttackStopper()override = default;

	// 入り、更新、抜け
	void Enter(Boss& boss)override;
	void Update(Boss& boss, float deltaTime)override;
	void Exit(Boss& boss) override;

private:
	// 落下中・着地済みの足止め1個ぶんの情報
	struct Stopper {
		AOENGINE::BaseEntity entity;		// 足止め本体
		bool isLanded = false;				// 足場に乗ったか
		float spawnX = 0.0f;				// 生成した時のX
		float spawnZ = 0.0f;				// 生成した時のZ
		float startY = 0.0f;				// 落下を始めた高さ
		float restY = 0.0f;					// 着地して止まる高さ
		float fallTimer = 0.0f;				// 落下を始めてからの経過時間
		float fallDuration = 0.0f;			// 落ちきるまでにかかる時間
		float remainingLifeTime = 0.0f;		// 着地してから消えるまでの残り時間
	};

private:

	/// <summary>
	/// プレイヤーから一番近い足場を除いた候補から、ランダムに選んで足止めを落とす
	/// </summary>
	void SpawnStoppers(const Boss& boss);

	/// <summary>
	/// 足止めを1個、指定した足場の上空に生成する
	/// </summary>
	void SpawnStopper(const Boss& boss, const Block& target);

	/// <summary>
	/// 落下・着地・寿命の管理
	/// </summary>
	void UpdateStoppers(Boss& boss, float deltaTime);

private:

	// 落下中・着地済みの足止め
	std::list<Stopper> stoppers_;

	bool isFinished_ = false;	// 終わったか

	// ブロックの表。落とす候補を選ぶのに使う
	StageBlockField* pBlockField_ = nullptr;

	// Collider category名
	const std::string kStopperName_ = "Stopper";

public:// acceccer

	bool IsFinished() const override { return isFinished_; }
	const std::string& GetName() const override {
		static const std::string kName = "BossAttackStopper";
		return kName;
	}

	// 落とす候補を選ぶためのブロックの表を設定する
	void SetBlockField(StageBlockField* field) { pBlockField_ = field; }
};

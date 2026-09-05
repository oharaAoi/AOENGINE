#pragma once

#include "Game/Actor/Boss/AttackBehavior/BaseBossAttackBehavior.h"
#include "Engine/Module/Components/GameObject/BaseEntity.h"
#include "Engine/Lib/Math/Vector3.h"
#include "Game/Stage/StageBlockField.h"
#include <list>
#include <vector>

class Block;

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
	// 落とし先の候補1つぶん
	struct LandingCandidate {
		int groupId = StageBlockField::kInvalidGroupId;	// グループID
		Block* landingBlock = nullptr;					// 実際に乗るブロック
		Math::Vector3 center{};							// グループの中心
	};

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
	/// プレイヤーがいるグループを除いた候補から、ランダムに選んで足止めを落とす
	/// </summary>
	void SpawnStoppers(const Boss& boss);

	/// <summary>
	/// カメラに映っている足場を連結グループごとにまとめ、落とし先の候補を作る
	/// </summary>
	std::vector<LandingCandidate> CollectCandidates(const Boss& boss) const;

	/// <summary>
	/// プレイヤーが乗っているグループを候補から外す。
	/// 同じグループのブロックは全て繋がっているので、隣接ブロックもまとめて外れる
	/// </summary>
	void ExcludePlayerGroup(std::vector<LandingCandidate>& candidates) const;

	/// <summary>
	/// 足止めを1個、指定した候補の上空に生成する
	/// </summary>
	void SpawnStopper(const Boss& boss, const LandingCandidate& target);

	/// <summary>
	/// 落下・着地・寿命の管理
	/// </summary>
	void UpdateStoppers(Boss& boss, float deltaTime);

private:

	// 落下中・着地済みの足止め
	std::list<Stopper> stoppers_;

	bool isFinished_ = false;	// 終わったか
	bool isSpawned_ = false;	// 待ちが明けて足止めを落としたか

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

	const std::string& GetAnimationName() const override {
		static const std::string kName = "attack3";
		return kName;
	}

	// 落とす候補を選ぶためのブロックの表を設定する
	void SetBlockField(StageBlockField* field) { pBlockField_ = field; }
};

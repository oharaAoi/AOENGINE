#pragma once

#include "Game/Actor/Boss/AttackBehavior/BaseBossAttackBehavior.h"
#include "Engine/Module/Components/GameObject/BaseEntity.h"
#include <list>

class BossAttackFallFire :public BaseBossAttackBehavior {
public:
	BossAttackFallFire() = default;
	~BossAttackFallFire()override = default;

	// 入り、更新、抜け
	void Enter(Boss& boss)override;
	void Update(Boss& boss, float deltaTime)override;
	void Exit(Boss& boss) override;

private:
	//FireBallの構造体
	struct Fireball {
		AOENGINE::BaseEntity entity;// 火玉本体
		bool isHit = false;// 何かに当たったか
	};

private:

	/// <summary>
	/// プレイヤーの上に火玉を生成して落下
	/// </summary>
	void SpawnFireball(const Boss& boss);

	/// <summary>
	/// スポーンタイマーの更新
	/// </summary>
	void UpdateSpawnTimer(float deltaTime,const Boss& boss);

	/// <summary>
	/// 火玉の更新
	/// </summary>
	void UpdateFireBall(float deltaTime);

	/// <summary>
	/// ボスがわっはっは跳ねるやつ
	/// </summary>
	/// <param name="boss"></param>
	/// <param name="deltaTime"></param>
	void UpdateBossBounce(const Boss& boss, float deltaTime);

private:

	// 落とす個数、発射間隔、スピード
	int dropCount_ = 3;
	float launchInterval_ = 0.5f;
	float fallSpeed_ = 10.0f;

	// 落下中の火玉
	std::list<Fireball> fireballs_;

	int spawnedCount_ = 0;// 出した火玉の数
	float spawnTimer_ = 0.0f;// 次のスポーンまでの時間
	bool isFinished_ = false;// 終わったか

	// bossMove
	float bounceTimer_ = 0.0f;//跳ねタイマー

	const std::string kFireBallColliderTag_ = "Fireball";
                      

public:// acceccer

	bool IsFinished() const override { return isFinished_; }
	const std::string& GetName() const override { return "BossAttackFallFire"; }
};
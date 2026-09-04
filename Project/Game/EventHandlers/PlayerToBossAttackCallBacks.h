#pragma once

/// engine
#include "Engine/System/Manager/CollisionManager.h"
#include "Engine/Module/Components/EventHandlers/BaseCollisionCallBacks.h"

// game
#include "Game/Actor/Player/Player.h"

class Boss;

/// <summary>
/// ボスの攻撃(火球・ビーム)がプレイヤーに当たった時のコールバック。
/// ダメージ量は BossParameter の攻撃ごとの調整値を使う。
/// </summary>
class PlayerToBossAttackCallBacks :
	public AOENGINE::BaseCollisionCallBacks {
public: // constructor

	PlayerToBossAttackCallBacks(AOENGINE::CollisionManager* _collisionManager) : pCollisionManager_(_collisionManager) {};
	~PlayerToBossAttackCallBacks() override = default;

public: // public method

	void Init() override;
	void Update() override {};

	void CollisionEnter(AOENGINE::BaseCollider* const player, AOENGINE::BaseCollider* const attack) override;
	void CollisionStay(AOENGINE::BaseCollider* const player, AOENGINE::BaseCollider* const attack) override;
	void CollisionExit(AOENGINE::BaseCollider* const player, AOENGINE::BaseCollider* const attack) override;

public: // accessor

	void SetPlayer(Player* player) { pPlayer_ = player; }
	void SetBoss(Boss* boss) { pBoss_ = boss; }

private: // private method

	/// <summary>当たった攻撃の種類に応じたダメージをプレイヤーへ与える</summary>
	void ApplyDamage(AOENGINE::BaseCollider* const attack);

private: // private variable

	Player* pPlayer_ = nullptr;
	Boss* pBoss_ = nullptr;
	AOENGINE::CollisionManager* pCollisionManager_;

	// Collider category名(Prefab側の設定と合わせる)
	const std::string kFireballName_ = "Fireball";
	const std::string kBeamName_ = "Beam";

};

#include "BossBlockLauncherCollisionCallBacks.h"

/// game
#include "Game/Actor/Boss/Boss.h"
#include "Game/WorldObject/Block.h"
#include "Game/Stage/BlockGroupLauncher.h"

using namespace AOENGINE;

void BossBlockLauncherCollisionCallBacks::Init(){
	SetCallBacks();
}

void BossBlockLauncherCollisionCallBacks::Update(){}

void BossBlockLauncherCollisionCallBacks::CollisionEnter(AOENGINE::BaseCollider* const bossCollider,AOENGINE::BaseCollider* const blockCollider){
	if(!bossCollider || !blockCollider || !pBoss_ || !pLauncher_){
		return;
	}

	// launcher全体のブロック数を取得してダメージ計算に使う
	BlockDamageCalculator::HitContext context{};
	context.blockCount = pLauncher_->GetBlockCount();
	pBoss_->Damage(damageCalculator_.Calculate(context));

}

void BossBlockLauncherCollisionCallBacks::CollisionStay(AOENGINE::BaseCollider* const /*bossCollider*/,AOENGINE::BaseCollider* const /*blockCollider*/){}
void BossBlockLauncherCollisionCallBacks::CollisionExit(AOENGINE::BaseCollider* const /*bossCollider*/,AOENGINE::BaseCollider* const /*blockCollider*/){}

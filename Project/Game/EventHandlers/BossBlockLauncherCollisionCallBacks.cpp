#include "BossBlockLauncherCollisionCallBacks.h"

/// game
#include "Game/Actor/Boss/Boss.h"
#include "Game/WorldObject/Block.h"
#include "Game/Stage/BlockGroupLauncherManager.h"

using namespace AOENGINE;

void BossBlockLauncherCollisionCallBacks::Init(){
	SetCallBacks();
}

void BossBlockLauncherCollisionCallBacks::Update(){}

void BossBlockLauncherCollisionCallBacks::CollisionEnter(AOENGINE::BaseCollider* const bossCollider,AOENGINE::BaseCollider* const blockCollider){
	if(!bossCollider || !blockCollider || !pBoss_ || !pLauncherManager_){
		return;
	}

	// 当たったブロックが属するランチャーのブロック数でダメージを決める。
	// 複数のランチャーが同時に飛んでいても、当たった塊の分だけが乗るようにする
	int blockCount = pLauncherManager_->GetBlockCountByCollider(blockCollider);
	if(blockCount <= 0){
		// 制御を手放した後のブロックなど、どのランチャーにも属していない場合は全体の数で代用する
		blockCount = pLauncherManager_->GetBlockCount();
	}

	BlockDamageCalculator::HitContext context{};
	context.blockCount = blockCount;
	pBoss_->Damage(damageCalculator_.Calculate(context));

}

void BossBlockLauncherCollisionCallBacks::CollisionStay(AOENGINE::BaseCollider* const /*bossCollider*/,AOENGINE::BaseCollider* const /*blockCollider*/){}
void BossBlockLauncherCollisionCallBacks::CollisionExit(AOENGINE::BaseCollider* const /*bossCollider*/,AOENGINE::BaseCollider* const /*blockCollider*/){}

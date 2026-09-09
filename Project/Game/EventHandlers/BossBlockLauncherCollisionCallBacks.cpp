#include "BossBlockLauncherCollisionCallBacks.h"

/// engine
#include "Engine/Core/Engine.h"
#include "Engine/System/Manager/ImGuiManager.h"

/// game
#include "Game/Actor/Boss/Boss.h"
#include "Game/WorldObject/Block.h"
#include "Game/Stage/BlockGroupLauncherManager.h"
#include "Engine/System/Manager/ParticleEffectManager.h"

using namespace AOENGINE;

void BossBlockLauncherCollisionCallBacks::Init(){
	SetCallBacks();

	// 保存済みのダメージ調整値を読み込む。呼ばないと既定値のままになる
	damageCalculator_.Load();

	// カメラの揺れ方も保存済みの値から読み込む
	cameraShakeParameter_.SetGroupName("BlockDamage");
	cameraShakeParameter_.SetName("bossHitCameraShake");
	cameraShakeParameter_.Load();
}

void BossBlockLauncherCollisionCallBacks::Update(){}

void BossBlockLauncherCollisionCallBacks::CollisionEnter(AOENGINE::BaseCollider* const bossCollider,AOENGINE::BaseCollider* const blockCollider){
	if(!bossCollider || !blockCollider || !pBoss_ || !pLauncherManager_){
		return;
	}

	// 当たったブロックが属するランチャーのブロック数でダメージを決める。
	// 複数のランチャーが同時に飛んでいても、当たった塊の分だけが乗るようにする
	int blockCount = pLauncherManager_->GetBlockCountByCollider(blockCollider);
	if(blockCount > 0){
		// 当たった塊はブロックごと消す(実際の破棄はランチャーの次の更新で行われる)。
		// 同じ塊の別のブロックが同じフレームに当たっても2回目以降は false が返るため、
		// 塊のブロック数だけダメージが重なることはない
		if(!pLauncherManager_->NotifyBossHit(blockCollider)){
			return;
		}
	} else{
		// 制御を手放した後のブロックなど、どのランチャーにも属していない場合は全体の数で代用する
		blockCount = pLauncherManager_->GetBlockCount();
	}

	BlockDamageCalculator::HitContext context{};
	context.blockCount = blockCount;
	context.groupCount = pLauncherManager_->GetGroupCount();
	pBoss_->Damage(damageCalculator_.Calculate(context));

	// 当たった手応えを出すためにカメラを揺らす
	pBoss_->ShakeCamera(cameraShakeParameter_);

	AOENGINE::ParticleEffectManager::GetInstance()->Play("BossHitEffect", blockCollider->GetCenterPos());

	// se
	Engine::GetSoundManager()->Play("BossDamaged");
}

void BossBlockLauncherCollisionCallBacks::CollisionStay(AOENGINE::BaseCollider* const /*bossCollider*/,AOENGINE::BaseCollider* const /*blockCollider*/){}
void BossBlockLauncherCollisionCallBacks::CollisionExit(AOENGINE::BaseCollider* const /*bossCollider*/,AOENGINE::BaseCollider* const /*blockCollider*/){}

void BossBlockLauncherCollisionCallBacks::Debug_Gui(){
	// 当たった時のカメラの揺れ方
	ImGui::SeparatorText("Boss Hit Camera Shake");
	cameraShakeParameter_.Debug_Gui();
	cameraShakeParameter_.SaveAndLoad();
	if(ImGui::Button("Test Play")){
		if(pBoss_){
			pBoss_->ShakeCamera(cameraShakeParameter_);
		}
	}
}

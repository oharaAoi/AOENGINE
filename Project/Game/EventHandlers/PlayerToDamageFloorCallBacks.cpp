#include "PlayerToDamageFloorCallBacks.h"

#include "Engine/Core/Engine.h"

#include "Game/Actor/Floor/DamageFloor.h"

void PlayerToDamageFloorCallBacks::Init() {
	SetCallBacks();
	SetPair(pCollisionManager_, "Player", "DamageFloor");
}

void PlayerToDamageFloorCallBacks::CollisionEnter(AOENGINE::BaseCollider* const , AOENGINE::BaseCollider* const ) {
	// ノックバック中はPlayer側のColliderを無効化
	if (!pPlayer_ || !pDamageFloor_) {
		return;
	}

	pPlayer_->ApplyDamageFloorKnockback(pDamageFloor_->GetKnockbackPower());

	// コネクト中のブロックグループを解除する
	pPlayer_->CancelBlockGroupConnect();

	Engine::GetSoundManager()->Play("PlayerDamageFloorCollision");
}

void PlayerToDamageFloorCallBacks::CollisionStay(AOENGINE::BaseCollider* const , AOENGINE::BaseCollider* const ) {
	// ヒットした瞬間だけ
}

void PlayerToDamageFloorCallBacks::CollisionExit(AOENGINE::BaseCollider* const , AOENGINE::BaseCollider* const ) {
	// ヒットした瞬間だけ
}

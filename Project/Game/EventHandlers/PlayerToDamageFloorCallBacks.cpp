#include "PlayerToDamageFloorCallBacks.h"


void PlayerToDamageFloorCallBacks::Init() {
	SetCallBacks();
	SetPair(pCollisionManager_, "Player", "DamageFloor");
}

void PlayerToDamageFloorCallBacks::CollisionEnter(AOENGINE::BaseCollider* const , AOENGINE::BaseCollider* const ) {
	// ココでPlayerのノックバック処理を呼び出す

}

void PlayerToDamageFloorCallBacks::CollisionStay(AOENGINE::BaseCollider* const , AOENGINE::BaseCollider* const ) {
	// ヒットした瞬間だけ
}

void PlayerToDamageFloorCallBacks::CollisionExit(AOENGINE::BaseCollider* const , AOENGINE::BaseCollider* const ) {
	// ヒットした瞬間だけ
}

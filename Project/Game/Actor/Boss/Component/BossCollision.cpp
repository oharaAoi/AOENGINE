#include "BossCollision.h"

#include "Engine/Module/Components/GameObject/BaseGameObject.h"
#include "Engine/Module/Components/Collider/BoxCollider.h"
#include "Engine/System/Manager/CollisionManager.h"

using namespace AOENGINE;

///////////////////////////////////////////////////////////////////////////////////////////////
//  Collider の生成
///////////////////////////////////////////////////////////////////////////////////////////////

void BossCollision::CreateCollider(BaseGameObject* body, const Math::Vector3& halfSize) {
	if (!body) {
		return;
	}

	// BoxColliderの生成
	collider_ = body->SetCollider(kCategory, ColliderShape::AABB);
	if (BoxCollider* box = dynamic_cast<BoxCollider*>(collider_)) {
		box->SetSize(halfSize);
	}
	// 
	if (collider_) {
		collider_->SetIsTrigger(true);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  衝突ペアの登録
///////////////////////////////////////////////////////////////////////////////////////////////

void BossCollision::RegisterPair(CollisionManager* collisionManager, const std::string& attackerCategory) {
	if (!collisionManager) {
		return;
	}
	// コールバックセット
	SetCallBacks();
	// ペアセット
	SetPair(collisionManager, attackerCategory, kCategory);
}

///////////////////////////////////////////////////////////////////////////////////////////////
//  衝突コールバック
///////////////////////////////////////////////////////////////////////////////////////////////

void BossCollision::CollisionEnter(BaseCollider* const attacker, BaseCollider* const boss) {
	(void)attacker;
	(void)boss;
	if (onHit_) {
		onHit_();
	}
}

void BossCollision::CollisionStay(BaseCollider* const attacker, BaseCollider* const boss) {
	(void)attacker;
	(void)boss;
}

void BossCollision::CollisionExit(BaseCollider* const attacker, BaseCollider* const boss) {
	(void)attacker;
	(void)boss;
}

#include "CollisionManager.h"
#include "Engine/Components/Collider/CollisionFunctions.h"
#include "Engine/Utilities/BitChecker.h"

CollisionManager::CollisionManager() {}
CollisionManager::~CollisionManager() {}


/////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　初期化処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void CollisionManager::Init() {
	colliders_.clear();
}

/////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　すべての当たり判定チェック
//////////////////////////////////////////////////////////////////////////////////////////////////

void CollisionManager::CheckAllCollision() {

	// リスト内のペアの総当たり判定
	std::list<ICollider*>::iterator iterA = colliders_.begin();
	for (; iterA != colliders_.end(); ++iterA) {
		ICollider* colliderA = *iterA;

		// 非アクティブなら次の要素に
		if (!colliderA->GetIsActive()) {
			continue;
		}

		// イテレータBはイテレータAの次の要素から回す
		std::list<ICollider*>::iterator iterB = iterA;
		iterB++;

		for (; iterB != colliders_.end(); ++iterB) {
			ICollider* colliderB = *iterB;

			// 非アクティブなら次の要素に
			if (!colliderB->GetIsActive()) {
				continue;
			}

			// マスク処理を行う
			if (!HasBit(colliderA->GetMaskBits(), colliderB->GetCategoryBit())) {
				continue;
			}

			// ペアの当たり判定
			CheckCollisionPair(colliderA, colliderB);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　コライダー2つの衝突判定と応答
/////////////////////////////////////////////////////////////////////////////////////////////////

void CollisionManager::CheckCollisionPair(ICollider* colliderA, ICollider* colliderB) {
	if (CheckCollision(colliderA->GetShape(), colliderB->GetShape())) {
		// Colliderの状態を変化させる
		colliderA->SwitchCollision(colliderB);
		colliderB->SwitchCollision(colliderA);

		OnCollision(colliderA, colliderB);
		
	} else {
		ExitCollision(colliderA, colliderB);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　ペアを作成する
/////////////////////////////////////////////////////////////////////////////////////////////////

void CollisionManager::MakeCollisionPair(uint32_t bitA, uint32_t bitB, const CallBackKinds& callBacks) {
	callBackFunctions_[CollisionPair(bitA, bitB)] = callBacks;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　衝突している時に行う関数
/////////////////////////////////////////////////////////////////////////////////////////////////

void CollisionManager::OnCollision(ICollider* colliderA, ICollider* colliderB) {
	// ペアを作成する
	auto pair = CollisionPair(colliderA->GetCategoryBit(), colliderB->GetCategoryBit());

	// ペアがマップに存在するかを確認
	auto it = callBackFunctions_.find(pair);
	if (it == callBackFunctions_.end()) {
		return;
	}

	const CallBackKinds& callbacks = it->second;

	// 状態にあった呼び出しを行う
	switch (colliderA->GetCollisionState()) {
	case CollisionFlags::ENTER:
		if (callbacks.enter) {
			callbacks.enter(colliderA, colliderB);
		}
		break;
	case CollisionFlags::STAY:
		if (callbacks.stay) {
			callbacks.stay(colliderA, colliderB);
		}
		break;
	default:
		break;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　衝突しなくなった瞬間に行う関数
/////////////////////////////////////////////////////////////////////////////////////////////////

void CollisionManager::ExitCollision(ICollider* colliderA, ICollider* colliderB) {
	// 衝突している状態だったら脱出した状態にする
	for (auto collider : { colliderA, colliderB }) {
		if (collider->GetCollisionState() == CollisionFlags::STAY) {
			collider->SetCollisionState(CollisionFlags::EXIT);
		} else {
			collider->SetCollisionState(CollisionFlags::NONE);
			collider->DeletePartner(colliderA == collider ? colliderB : colliderA);
		}
	}
}

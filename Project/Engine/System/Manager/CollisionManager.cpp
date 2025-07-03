#include "CollisionManager.h"
#include "Engine/Module/Components/Collider/CollisionFunctions.h"
#include "Engine/System/Collision/PenetrationResolution.h"
#include "Engine/Utilities/BitChecker.h"

CollisionManager::CollisionManager() {}
CollisionManager::~CollisionManager() {
	pColliderCollector_->Reset();
}

/////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　初期化処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void CollisionManager::Init() {
	pColliderCollector_ = ColliderCollector::GetInstance();
	pColliderCollector_->Init();
}

/////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　すべての当たり判定チェック
//////////////////////////////////////////////////////////////////////////////////////////////////

void CollisionManager::CheckAllCollision() {
	
	std::list<ICollider*>& colliderList = pColliderCollector_->GetColliderList();

	// リスト内のペアの総当たり判定
	std::list<ICollider*>::iterator iterA = colliderList.begin();
	for (; iterA != colliderList.end(); ++iterA) {
		ICollider* colliderA = *iterA;

		// 非アクティブなら次の要素に
		if (!colliderA->GetIsActive()) {
			continue;
		}

		// イテレータBはイテレータAの次の要素から回す
		std::list<ICollider*>::iterator iterB = iterA;
		iterB++;

		for (; iterB != colliderList.end(); ++iterB) {
			ICollider* colliderB = *iterB;

			// 非アクティブなら次の要素に
			if (!colliderB->GetIsActive()) {
				continue;
			}

			// マスク処理を行う
			if (colliderA->GetCategoryName() != "none" && colliderB->GetCategoryName() != "none") {
				if (!HasBit(colliderA->GetMaskBits(), colliderB->GetCategoryBit())) {
					continue;
				}
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

		// 汎用の当たり判定後処理
		colliderA->OnCollision(colliderB);
		colliderB->OnCollision(colliderA);

		if (!colliderA->GetIsStatic()) {
			colliderA->SetPushBackDirection(PenetrationResolution(colliderA->GetShape(), colliderB->GetShape()));
		}

		if (!colliderB->GetIsStatic()) {
			colliderB->SetPushBackDirection(PenetrationResolution(colliderA->GetShape(), colliderB->GetShape()));
		}
		
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
	auto reversePair = CollisionPair(colliderB->GetCategoryBit(), colliderA->GetCategoryBit());

	// ペアがマップに存在するかを確認
	bool isReverse = false;
	auto it = callBackFunctions_.find(pair);
	auto reverseIt = callBackFunctions_.find(reversePair);

	if (it == callBackFunctions_.end()) {
		isReverse = true;
		if (reverseIt == callBackFunctions_.end()) {
			return;
		}
	}
	
	std::pair<ICollider*, ICollider*> collisionPair;
	CallBackKinds callbacks;
	if (isReverse) {
		callbacks = reverseIt->second;
		collisionPair.first = colliderB;
		collisionPair.second = colliderA;
	} else {
		callbacks = it->second;
		collisionPair.first = colliderA;
		collisionPair.second = colliderB;
	}

	switch (collisionPair.first->GetCollisionState()) {
	case CollisionFlags::ENTER:
		if (callbacks.enter) {
			callbacks.enter(collisionPair.first, collisionPair.second);
		}
		break;
	case CollisionFlags::STAY:
		if (callbacks.stay) {
			callbacks.stay(collisionPair.first, collisionPair.second);
		}
		break;
	default:
		break;
	}

	switch (collisionPair.second->GetCollisionState()) {
	case CollisionFlags::ENTER:
		if (callbacks.enter) {
			callbacks.enter(collisionPair.first, collisionPair.second);
		}
		break;
	case CollisionFlags::STAY:
		if (callbacks.stay) {
			callbacks.stay(collisionPair.first, collisionPair.second);
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

void CollisionManager::CallBackCollision(ICollider* colliderA, ICollider* colliderB, CallBackKinds callBack) {
	switch (colliderA->GetCollisionState()) {
	case CollisionFlags::ENTER:
		if (callBack.enter) {
			callBack.enter(colliderA, colliderB);
		}
		break;
	case CollisionFlags::STAY:
		if (callBack.stay) {
			callBack.stay(colliderA, colliderB);
		}
		break;
	default:
		break;
	}
}

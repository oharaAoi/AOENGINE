#include "BossBulletManager.h"

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化する
///////////////////////////////////////////////////////////////////////////////////////////////

void BossBulletManager::Init() {
	bulletList_.clear();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新する
///////////////////////////////////////////////////////////////////////////////////////////////

void BossBulletManager::Update() {
	// フラグがfalseになったら削除
	std::erase_if(bulletList_, [](auto& bullet) {
		return !bullet->GetIsAlive();
				  });

	for (auto& bullet : bulletList_) {
		bullet->SetTargetPosition(playerPosition_);
		bullet->Update();
	}
}

BaseBullet* BossBulletManager::SearchCollider(ICollider* collider) {
	for (const auto& bullet : bulletList_) {
		if (bullet->GetCollider() == collider) {
			return bullet.get();
		}
	}
	return nullptr;
}

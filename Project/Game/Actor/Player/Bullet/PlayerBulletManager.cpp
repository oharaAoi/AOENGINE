#include "PlayerBulletManager.h"

PlayerBulletManager::~PlayerBulletManager() {
	bulletList_.clear();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerBulletManager::Init() {
	bulletList_.clear();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerBulletManager::Update(const Vector3& playerTargetPos) {
	// フラグがfalseになったら削除
	std::erase_if(bulletList_, [](const std::unique_ptr<BaseBullet>& bullet) {
		return !bullet->GetIsAlive();
				  });

	for (std::unique_ptr<BaseBullet>& bullet : bulletList_) {
		if (bullet->GetBulletType() == MISSILE) {
			bullet->SetTargetPosition(playerTargetPos);
		}
		bullet->Update();
	}
}

BaseBullet* PlayerBulletManager::SearchCollider(ICollider* collider) {
	for (std::unique_ptr<BaseBullet>& bullet : bulletList_) {
		if (bullet->GetCollider() == collider) {
			return bullet.get();
		}
	}

	return nullptr;
}

#include "EnemyBulletManager.h"

EnemyBulletManager::~EnemyBulletManager() {
	bulletList_.clear();
}

void EnemyBulletManager::Init() {
	bulletList_.clear();
}

void EnemyBulletManager::Update() {
	CheckBulletLife();

	for (auto& bullet : bulletList_) {
		bullet->SetTargetPosition(playerPosition_);
		bullet->Update();
	}
}

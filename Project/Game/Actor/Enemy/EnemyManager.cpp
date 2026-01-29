#include "EnemyManager.h"

void EnemyManager::Init() {

}

void EnemyManager::Update() {
	for (auto& enemy : enemyList_) {
		enemy->Update();
	}
}
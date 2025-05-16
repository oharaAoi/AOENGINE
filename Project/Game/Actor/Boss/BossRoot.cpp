#include "BossRoot.h"

void BossRoot::Init() {
	boss_ = std::make_unique<Boss>();
	boss_->Init();

	bulletManager_ = std::make_unique<BossBulletManager>();
	bulletManager_->Init();

}

void BossRoot::Update() {
	boss_->Update();

	bulletManager_->Update();
}

void BossRoot::Draw() const {
	boss_->Draw();

	bulletManager_->Draw();
}

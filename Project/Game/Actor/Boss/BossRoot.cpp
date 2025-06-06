#include "BossRoot.h"

void BossRoot::Init() {
	boss_ = std::make_unique<Boss>();
	boss_->Init();

	bulletManager_ = std::make_unique<BossBulletManager>();
	bulletManager_->Init();

	// 初期設定
	boss_->SetBulletManager(bulletManager_.get());
}

void BossRoot::Update() {
	boss_->SetPlayerPosition(playerPosition_);
	boss_->Update();

	bulletManager_->SetPlayerPosition(playerPosition_);
	bulletManager_->Update();
}

void BossRoot::Draw() const {
	Engine::SetPipeline(PSOType::Object3d, "Object_Normal.json");
	boss_->Draw();

	bulletManager_->Draw();
}

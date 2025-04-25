#include "PlayerBulletManager.h"

PlayerBulletManager::~PlayerBulletManager() {
	bulletList_.clear();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerBulletManager::Init() {
	bulletList_.clear();

	hitBossSmoke_ = std::make_unique<HitBossSmoke>();
	hitBossSmoke_->Init("HitBossSmoke");

	hitBossSmokeBorn_ = std::make_unique<HitBossSmokeBorn>();
	hitBossSmokeBorn_->Init("HitBossSmokeBorn");

	hitBossExploadParticles_ = std::make_unique<HitBossExploadParticles>();
	hitBossExploadParticles_->Init("HitBossExploadParticles");
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerBulletManager::Update() {
	// フラグがfalseになったら削除
	std::erase_if(bulletList_, [](const PlayerBullet& bullet) {
		return !bullet.GetIsAlive();
				  });

	for (PlayerBullet& bullet : bulletList_) {
		bullet.Update();
	}

	hitBossSmoke_->Update(Render::GetCameraRotate());
	hitBossSmokeBorn_->Update(Render::GetCameraRotate());
	hitBossExploadParticles_->Update(Render::GetCameraRotate());
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 描画
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerBulletManager::Draw() const {
	for (const PlayerBullet& bullet : bulletList_) {
		bullet.Draw();
	}
}

void PlayerBulletManager::CollisionToBoss(const Vector3& bossPos) {
	for (PlayerBullet& bullet : bulletList_) {
		float length = (bullet.GetTransform()->translate_ - bossPos).Length();

		if (length < 4.0f) {
			// effectを出す
			bullet.SetIsAlive(false);

			hitBossExploadParticles_->SetPos(bullet.GetTransform()->translate_);
			hitBossExploadParticles_->SetOnShot();

			hitBossSmoke_->SetPos(bullet.GetTransform()->translate_);
			hitBossSmoke_->SetOnShot();

			hitBossSmokeBorn_->SetPos(bullet.GetTransform()->translate_);
			hitBossSmokeBorn_->SetOnShot();
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 弾を追加する
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerBulletManager::AddBullet(const Vector3& pos, const Vector3& velocity) {
	PlayerBullet& bullet = bulletList_.emplace_back();
	bullet.Init();
	bullet.Reset(pos, velocity);
}

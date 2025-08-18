#include "PlayerBullet.h"
#include "Game/Information/ColliderCategory.h"

PlayerBullet::~PlayerBullet() {
	BaseBullet::Finalize();
}

void PlayerBullet::Init() {
	BaseBullet::Init("PlayerBullet");
	object_->SetObject("playerBullet.obj");
	object_->SetCollider(ColliderTags::Bullet::machinegun, ColliderShape::SPHERE);

	ICollider* collider = object_->GetCollider(ColliderTags::Bullet::machinegun);
	collider->SetTarget(ColliderTags::Boss::own);
	collider->SetOnCollision([this](ICollider* other) { OnCollision(other); });

	type_ = BulletType::NORMAL;
}

void PlayerBullet::Update() {
	if (std::abs(transform_->srt_.translate.x) >= 200.0f) {
		isAlive_ = false;
	}

	if (std::abs(transform_->srt_.translate.y) >= 200.0f) {
		isAlive_ = false;
	}

	if (std::abs(transform_->srt_.translate.z) >= 200.0f) {
		isAlive_ = false;
	}

	BaseBullet::Update();
}

void PlayerBullet::OnCollision(ICollider* other) {
	if (other->GetCategoryName() == ColliderTags::None::own) {
		isAlive_ = false;
		BaseParticles* hitEffect = ParticleManager::GetInstance()->CrateParticle("MissileHit");
		hitEffect->SetPos(object_->GetPosition());
		hitEffect->Reset();
	}
}
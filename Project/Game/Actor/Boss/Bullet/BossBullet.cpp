#include "BossBullet.h"
#include "Game/Information/ColliderCategory.h"

BossBullet::~BossBullet() {
	BaseBullet::Finalize();
}

void BossBullet::Init() {
	BaseBullet::Init("BossBullet");
	object_->SetObject("playerBullet.obj");
	object_->SetCollider(ColliderTags::Boss::missile, ColliderShape::SPHERE);

	ICollider* collider = object_->GetCollider(ColliderTags::Boss::missile);
	collider->SetTarget(ColliderTags::Player::own);
	collider->SetTarget(ColliderTags::None::own);
	collider->SetOnCollision([this](ICollider* other) { OnCollision(other); });
	//collider->SetTarget(ColliderTags::Field::ground);
}

void BossBullet::Update() {
	if (std::abs(transform_->translate_.x) >= 200.0f) {
		isAlive_ = false;
	}

	if (std::abs(transform_->translate_.y) >= 200.0f) {
		isAlive_ = false;
	}

	if (std::abs(transform_->translate_.z) >= 200.0f) {
		isAlive_ = false;
	}

	BaseBullet::Update();
}

void BossBullet::Reset(const Vector3& pos, const Vector3& velocity) {
	transform_->translate_ = pos;
	velocity_ = velocity;
}

void BossBullet::OnCollision(ICollider* other) {
	if (other->GetCategoryName() == ColliderTags::None::own) {
		isAlive_ = false;
		BaseParticles* hitEffect = ParticleManager::GetInstance()->CrateParticle("MissileHit");
		hitEffect->SetPos(object_->GetPosition());
		hitEffect->Reset();
	}
}
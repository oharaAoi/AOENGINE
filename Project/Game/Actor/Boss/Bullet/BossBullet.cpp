#include "BossBullet.h"
#include "Game/Information/ColliderCategory.h"

BossBullet::~BossBullet() {
	BaseBullet::Finalize();
}

void BossBullet::Init() {
	BaseBullet::Init("BossBullet");
	bullet_->SetObject("missile.obj");
	bullet_->SetCollider(ColliderTags::Boss::missile, ColliderShape::SPHERE);

	ICollider* collider = bullet_->GetCollider();
	collider->SetTarget(ColliderTags::Player::own);
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

void BossBullet::Reset(const Vector3& pos, const Vector3& velocity, float bulletSpeed) {
	transform_->translate_ = pos;
	velocity_ = velocity;
	speed_ = bulletSpeed;
}

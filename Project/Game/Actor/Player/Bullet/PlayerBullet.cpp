#include "PlayerBullet.h"
#include "Game/Information/ColliderCategory.h"

PlayerBullet::~PlayerBullet() {
	BaseBullet::Finalize();
}

void PlayerBullet::Init() {
	BaseBullet::Init("PlayerBullet");
	object_->SetObject("playerBullet.obj");
	object_->SetCollider(ColliderTags::Bullet::machinegun, ColliderShape::SPHERE);

	ICollider* collider = object_->GetCollider();
	collider->SetTarget(ColliderTags::Boss::own);
}

void PlayerBullet::Update() {
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


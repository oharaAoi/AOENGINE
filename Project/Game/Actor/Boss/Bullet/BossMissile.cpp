#include "BossMissile.h"
#include "Game/Information/ColliderCategory.h"

BossMissile::~BossMissile() {
	BaseBullet::Finalize();
}

void BossMissile::Init() {
	BaseBullet::Init();
	SetObject("missile.obj");
	SetCollider(ColliderTags::Boss::missile, ColliderShape::SPHERE);
	collider_->SetTarget(ColliderTags::Boss::missile);
}

void BossMissile::Update() {
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

void BossMissile::Draw() const {
	BaseBullet::Draw();
}

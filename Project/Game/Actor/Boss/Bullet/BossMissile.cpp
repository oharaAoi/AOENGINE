#include "BossMissile.h"
#include "Game/Information/ColliderCategory.h"

BossMissile::~BossMissile() {
	BaseBullet::Finalize();
}

void BossMissile::Init() {
	BaseBullet::Init();
	SetObject("missile.obj");
	SetCollider(ColliderTags::Boss::missile, ColliderShape::SPHERE);
	collider_->SetTarget(ColliderTags::Player::own);
	collider_->SetTarget(ColliderTags::Field::ground);

	trackingLength_ = 10.f;
	trackingTimer_ = 0.f;

	finishTracking_ = false;
}

void BossMissile::Update() {

	Tracking();
	
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

void BossMissile::Reset(const Vector3& pos, const Vector3& velocity, const Vector3& targetPosition, float bulletSpeed) {
	transform_->translate_ = pos;
	velocity_ = velocity;
	targetPosition_ = targetPosition;
	speed_ = bulletSpeed;
}

void BossMissile::Tracking() {
	if (finishTracking_) { return; }

	if ((targetPosition_ - transform_->translate_).Length() > trackingLength_) {
		// 最初の数秒は追尾しない
		trackingTimer_ += GameTimer::DeltaTime();
		if (trackingTimer_ < trackingTime_) { return; }

		// targetの方向に弾を撃つ
		Vector3 targetToDire = (targetPosition_ - transform_->translate_).Normalize() * speed_;
		velocity_ = Vector3::Lerp(velocity_, targetToDire, 0.05f);
	} else {
		finishTracking_ = true;
	}
}

#include "BossMissile.h"
#include "Game/Information/ColliderCategory.h"
#include "Engine/System/Manager/ParticleManager.h"

BossMissile::~BossMissile() {
	BaseBullet::Finalize();
	//ParticleManager::GetInstance()->DeleteParticles(burn_);
	burn_ = nullptr;
	smoke_ = nullptr;
}

void BossMissile::Init() {
	BaseBullet::Init("BossMissile");
	bullet_->SetObject("missile.obj");
	bullet_->SetCollider(ColliderTags::Boss::missile, ColliderShape::SPHERE);

	ICollider* collider = bullet_->GetCollider();
	collider->SetTarget(ColliderTags::Player::own);
	//collider->SetTarget(ColliderTags::Field::ground);

	trackingLength_ = 10.f;
	trackingTimer_ = 0.f;

	finishTracking_ = false;

	//burn_ = ParticleManager::GetInstance()->CrateParticle("MissileBurn");
	//smoke_ = ParticleManager::GetInstance()->CrateParticle("MissileBurnSmoke");
	//burn_->SetParent(transform_->GetWorldMatrix());
	//smoke_->SetParent(transform_->GetWorldMatrix());
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
//	burn_->Update();
	//smoke_->Update();
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

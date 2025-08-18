#include "RocketBullet.h"
#include "Game/Information/ColliderCategory.h"

RocketBullet::~RocketBullet() {
	BaseBullet::Finalize();
	ParticleManager::GetInstance()->DeleteParticles(burn_);
	ParticleManager::GetInstance()->DeleteParticles(smoke_);
	burn_ = nullptr;
	smoke_ = nullptr;
}

void RocketBullet::Init() {
	BaseBullet::Init("RocketMissile");
	object_->SetObject("missile.obj");
	object_->SetCollider(ColliderTags::Bullet::rocket, ColliderShape::SPHERE);

	ICollider* collider = object_->GetCollider(ColliderTags::Bullet::rocket);
	collider->SetTarget(ColliderTags::Boss::own);
	collider->SetTarget(ColliderTags::Field::ground);
	collider->SetTarget(ColliderTags::None::own);
	collider->SetOnCollision([this](ICollider* other) { OnCollision(other); });

	trackingLength_ = 10.f;
	trackingTimer_ = 0.f;
	finishTracking_ = false;

	burn_ = ParticleManager::GetInstance()->CrateParticle("MissileBurn");
	smoke_ = ParticleManager::GetInstance()->CrateParticle("MissileBurnSmoke");
	burn_->SetParent(transform_->GetWorldMatrix());
	smoke_->SetParent(transform_->GetWorldMatrix());
	burn_->SetIsStop(false);
	smoke_->SetIsStop(false);

	type_ = BulletType::MISSILE;
}

void RocketBullet::Update() {
	Tracking();

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

void RocketBullet::OnCollision(ICollider* other) {
	if (other->GetCategoryName() == ColliderTags::None::own) {
		isAlive_ = false;
		BaseParticles* hitEffect = ParticleManager::GetInstance()->CrateParticle("MissileHit");
		hitEffect->SetPos(object_->GetPosition());
		hitEffect->Reset();
	}
}

void RocketBullet::Reset(const Vector3& pos, const Vector3& target, float bulletSpeed) {
	transform_->srt_.translate = pos;
	targetPosition_ = target;
	speed_ = bulletSpeed;
}

void RocketBullet::Tracking() {
	if (finishTracking_) { return; }

	if ((targetPosition_ - transform_->srt_.translate).Length() > trackingLength_) {
		// targetの方向に弾を撃つ
		Vector3 targetToDire = (targetPosition_ - transform_->srt_.translate).Normalize() * speed_;
		velocity_ = Vector3::Lerp(velocity_, targetToDire, trackingRaito_);

		if (trackingTimer_ < trackingTime_) {
			trackingTimer_ += GameTimer::DeltaTime();
			velocity_.y = speed_;
		}
	} else {
		finishTracking_ = true;
	}
}
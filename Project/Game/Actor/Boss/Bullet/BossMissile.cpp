#include "BossMissile.h"
#include "Game/Information/ColliderCategory.h"
#include "Engine/System/Manager/ParticleManager.h"
#include "Engine/Lib/Math/Easing.h"

BossMissile::~BossMissile() {
	BaseBullet::Finalize();
	ParticleManager::GetInstance()->DeleteParticles(burn_);
	ParticleManager::GetInstance()->DeleteParticles(smoke_);
	burn_ = nullptr;
	smoke_ = nullptr;
}

void BossMissile::Init() {
	BaseBullet::Init("BossMissile");
	object_->SetObject("missile.obj");
	object_->SetCollider(ColliderTags::Boss::missile, ColliderShape::SPHERE);

	ICollider* collider = object_->GetCollider(ColliderTags::Boss::missile);
	collider->SetTarget(ColliderTags::Player::own);
	collider->SetTarget(ColliderTags::Field::ground);
	collider->SetTarget(ColliderTags::None::own);
	collider->SetOnCollision([this](ICollider* other) { OnCollision(other); });

	trackingLength_ = 5.f;
	trackingTimer_ = 0.f;

	finishTracking_ = false;

	accelTimer_ = Timer(1.0f);

	burn_ = ParticleManager::GetInstance()->CrateParticle("MissileBurn");
	smoke_ = ParticleManager::GetInstance()->CrateParticle("MissileBurnSmoke");
	burn_->SetParent(transform_->GetWorldMatrix());
	smoke_->SetParent(transform_->GetWorldMatrix());
	burn_->SetIsStop(false);
	smoke_->SetIsStop(false);
}

void BossMissile::Update() {
	Accelerate();
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


void BossMissile::Reset(const Vector3& pos, const Vector3& velocity, const Vector3& targetPosition,
						float bulletSpeed, float firstSpeedRaito, float trackingRaito, bool isTracking) {
	transform_->srt_.translate = pos;
	velocity_ = velocity;
	targetPosition_ = targetPosition;
	targetSpeed_ = bulletSpeed;
	firstSpeedRaito_ = firstSpeedRaito;
	trackingRaito_ = trackingRaito;

	if (!isTracking) {
		finishTracking_ = true;
		velocity_ = (targetPosition_ - transform_->srt_.translate).Normalize() * targetSpeed_;
	}
}

void BossMissile::Tracking() {
	if (finishTracking_) { return; }

	if ((targetPosition_ - transform_->srt_.translate).Length() > trackingLength_) {
		// 最初の数秒は追尾しない
		trackingTimer_ += GameTimer::DeltaTime();
		if (trackingTimer_ < trackingTime_) { return; }

		// targetの方向に弾を撃つ
		Vector3 targetToDire = (targetPosition_ - transform_->srt_.translate).Normalize() * speed_;
		velocity_ = Vector3::Lerp(velocity_, targetToDire, trackingRaito_);
	} else {
		finishTracking_ = true;
	}
}

void BossMissile::Accelerate() {
	if (accelTimer_.Run(GameTimer::DeltaTime())) {
		speed_ = std::lerp(targetSpeed_ * firstSpeedRaito_, targetSpeed_, Ease::In::Quart(accelTimer_.t_));
		velocity_ = velocity_.Normalize() * speed_;
	}
}

void BossMissile::OnCollision(ICollider* other) {
	if (other->GetCategoryName() == ColliderTags::None::own) {
		isAlive_ = false;
		BaseParticles* hitEffect = ParticleManager::GetInstance()->CrateParticle("MissileHit");
		hitEffect->SetPos(object_->GetPosition());
		hitEffect->Reset();
	}
}
#include "BossMissile.h"
#include "Game/Information/ColliderCategory.h"
#include "Engine/System/Manager/ParticleManager.h"
#include "Engine/Lib/GameTimer.h"
#include "Engine/Lib/Math/Easing.h"
#include "Engine/Lib/GameTimer.h"

BossMissile::~BossMissile() {
	BaseBullet::Finalize();
	AOENGINE::ParticleManager::GetInstance()->DeleteParticles(burn_);
	AOENGINE::ParticleManager::GetInstance()->DeleteParticles(smoke_);
	burn_ = nullptr;
	smoke_ = nullptr;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossMissile::Init() {
	BaseBullet::Init("BossMissile");
	object_->SetObject("missile.obj");
	object_->SetCollider(ColliderTags::Boss::missile, ColliderShape::Sphere);

	AOENGINE::BaseCollider* collider = object_->GetCollider(ColliderTags::Boss::missile);
	collider->SetTarget(ColliderTags::Player::own);
	collider->SetTarget(ColliderTags::Field::ground);
	collider->SetTarget(ColliderTags::Field::building);
	collider->SetTarget(ColliderTags::None::own);
	collider->SetOnCollision([this](AOENGINE::BaseCollider* other) { OnCollision(other); });

	trackingTimer_ = 0.f;

	finishTracking_ = false;

	accelTimer_ = AOENGINE::Timer(1.0f);
	lifeTimer_ = AOENGINE::Timer(lifeTime_);

	shotFrea_ = AOENGINE::ParticleManager::GetInstance()->CrateParticle("shotFrea");
	burn_ = AOENGINE::ParticleManager::GetInstance()->CrateParticle("MissileBurn");
	smoke_ = AOENGINE::ParticleManager::GetInstance()->CrateParticle("cloud");
	burn_->SetParent(transform_->GetWorldMatrix());
	smoke_->SetParent(transform_->GetWorldMatrix());
	burn_->SetIsStop(false);
	smoke_->SetIsStop(false);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossMissile::Update() {
	Accelerate();
	Tracking();

	if (lifeTimer_.Run(AOENGINE::GameTimer::DeltaTime())) {
		isAlive_ = false;
	}
	
	if (std::abs(transform_->srt_.translate.x) >= deadLength_) {
		isAlive_ = false;
	}

	if (std::abs(transform_->srt_.translate.y) >= deadLength_) {
		isAlive_ = false;
	}

	if (std::abs(transform_->srt_.translate.z) >= deadLength_) {
		isAlive_ = false;
	}

	BaseBullet::Update();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ リセット処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossMissile::Reset(const Math::Vector3& pos, const Math::Vector3& velocity, const Math::Vector3& targetPosition,
						float bulletSpeed, float firstSpeedRaito, float trackingRaito, bool isTracking) {
	transform_->srt_.translate = pos;
	velocity_ = velocity;
	targetPosition_ = targetPosition;
	targetSpeed_ = bulletSpeed;
	firstSpeedRaito_ = firstSpeedRaito;
	trackingRaito_ = trackingRaito;
	trackingFinishTimer_.timer_ = 0;
	trackingFinishTimer_.targetTime_ = trackingFinishTime_;

	if (!isTracking) {
		finishTracking_ = true;
		velocity_ = (targetPosition_ - transform_->srt_.translate).Normalize() * targetSpeed_;
	}

	shotFrea_->SetPos(pos);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 追従処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossMissile::Tracking() {
	if (finishTracking_) { return; }

	if ((targetPosition_ - transform_->srt_.translate).Length() > trackingLength_) {
		// 最初の数秒は追尾しない
		trackingTimer_ += AOENGINE::GameTimer::DeltaTime();
		if (trackingTimer_ < trackingTime_) { return; }

		// targetの方向に弾を撃つ
		Math::Vector3 targetToDire = (targetPosition_ - transform_->srt_.translate).Normalize() * speed_;
		velocity_ = Math::Vector3::Lerp(velocity_, targetToDire, trackingRaito_);
	} else {
		finishTracking_ = true;
	}

	if (!trackingFinishTimer_.Run(AOENGINE::GameTimer::DeltaTime())) {
		finishTracking_ = true;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 加速処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossMissile::Accelerate() {
	if (accelTimer_.Run(AOENGINE::GameTimer::DeltaTime())) {
		speed_ = std::lerp(targetSpeed_ * firstSpeedRaito_, targetSpeed_, Ease::In::Quart(accelTimer_.t_));
		velocity_ = velocity_.Normalize() * speed_;
	}
}

void BossMissile::OnCollision(AOENGINE::BaseCollider* other) {
	if (other->GetCategoryName() == ColliderTags::None::own || other->GetCategoryName() == ColliderTags::Field::building) {
		isAlive_ = false;
		AOENGINE::BaseParticles* hitEffect = AOENGINE::ParticleManager::GetInstance()->CrateParticle("MissileHit");
		hitEffect->SetPos(object_->GetPosition());
		hitEffect->Reset();
	}
}
#include "RocketBullet.h"
#include "Game/Information/ColliderCategory.h"
#include "Engine/System/Audio/AudioPlayer.h"
#include "Engine/Lib/GameTimer.h"

RocketBullet::~RocketBullet() {
	BaseBullet::Finalize();
	ParticleManager::GetInstance()->DeleteParticles(burn_);
	ParticleManager::GetInstance()->DeleteParticles(smoke_);
	burn_ = nullptr;
	smoke_ = nullptr;
}

void RocketBullet::BulletParam::Debug_Gui() {
	ImGui::DragFloat("hitSeVolume", &hitSeVolume, 0.1f);
	SaveAndLoad();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void RocketBullet::Init() {
	BaseBullet::Init("RocketMissile");
	object_->SetObject("missile.obj");
	object_->SetCollider(ColliderTags::Bullet::rocket, ColliderShape::Sphere);

	// ----------------------
	// ↓ colliderの設定
	// ----------------------
	AOENGINE::BaseCollider* collider = object_->GetCollider(ColliderTags::Bullet::rocket);
	collider->SetTarget(ColliderTags::Boss::own);
	collider->SetTarget(ColliderTags::Field::ground);
	collider->SetTarget(ColliderTags::None::own);
	collider->SetOnCollision([this](AOENGINE::BaseCollider* other) { OnCollision(other); });
	collider->SetIsTrigger(true);

	trackingTimer_ = 0.f;
	finishTracking_ = false;

	// ----------------------
	// ↓ effectの設定
	// ----------------------
	burn_ = ParticleManager::GetInstance()->CrateParticle("MissileBurn");
	smoke_ = ParticleManager::GetInstance()->CrateParticle("cloud");
	burn_->SetParent(transform_->GetWorldMatrix());
	smoke_->SetParent(transform_->GetWorldMatrix());
	burn_->SetIsStop(false);
	smoke_->SetIsStop(false);

	type_ = BulletType::Missile;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ コリジョン時の処理
///////////////////////////////////////////////////////////////////////////////////////////////

void RocketBullet::OnCollision(AOENGINE::BaseCollider* other) {
	if (other->GetCategoryName() == ColliderTags::None::own || other->GetCategoryName() == ColliderTags::Boss::own) {
		isAlive_ = false;
		AOENGINE::BaseParticles* hitEffect = ParticleManager::GetInstance()->CrateParticle("MissileHit");
		hitEffect->SetPos(object_->GetPosition());
		hitEffect->Reset();

		AOENGINE::AudioPlayer::SinglShotPlay("missileHit.mp3", param_.hitSeVolume);
	}
}

void RocketBullet::Reset(const Math::Vector3& _pos, const Math::Vector3& _target, float _bulletSpeed, float _trackingLength, float _trackingTime, float _trackingRaito) {
	transform_->srt_.translate = _pos;
	targetPosition_ = _target;
	speed_ = _bulletSpeed;
	trackingLength_ = _trackingLength;
	trackingTime_ = _trackingTime;
	trackingRaito_ = _trackingRaito;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 追従処理
///////////////////////////////////////////////////////////////////////////////////////////////

void RocketBullet::Tracking() {
	if (finishTracking_) { return; }

	if ((targetPosition_ - transform_->srt_.translate).Length() > trackingLength_) {
		// targetの方向に弾を撃つ
		Math::Vector3 targetToDire = (targetPosition_ - transform_->srt_.translate).Normalize() * speed_;
		velocity_ = Math::Vector3::Lerp(velocity_, targetToDire, trackingRaito_);

		if (trackingTimer_ < trackingTime_) {
			trackingTimer_ += AOENGINE::GameTimer::DeltaTime();
			velocity_.y = speed_;
		}
	} else {
		finishTracking_ = true;
	}
}
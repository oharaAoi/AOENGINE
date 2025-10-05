#include "LauncherBullet.h"
#include "Game/Information/ColliderCategory.h"

LauncherBullet::~LauncherBullet() {
	BaseBullet::Finalize();
	ParticleManager::GetInstance()->DeleteParticles(burn_);
	ParticleManager::GetInstance()->DeleteParticles(smoke_);
	burn_ = nullptr;
	smoke_ = nullptr;
}

void LauncherBullet::Init() {
	BaseBullet::Init("LauncherBullet");
	object_->SetObject("missile.obj");
	object_->SetCollider(ColliderTags::Bullet::rocket, ColliderShape::SPHERE);

	ICollider* collider = object_->GetCollider(ColliderTags::Bullet::rocket);
	collider->SetTarget(ColliderTags::Boss::own);
	collider->SetTarget(ColliderTags::Field::ground);
	collider->SetTarget(ColliderTags::None::own);
	collider->SetOnCollision([this](ICollider* other) { OnCollision(other); });

	speed_ = 90.0f;

	burn_ = ParticleManager::GetInstance()->CrateParticle("MissileBurn");
	smoke_ = ParticleManager::GetInstance()->CrateParticle("Launcher");
	burn_->SetParent(transform_->GetWorldMatrix());
	smoke_->SetParent(transform_->GetWorldMatrix());
	burn_->SetIsStop(false);
	smoke_->SetIsStop(false);

	type_ = BulletType::LAUNCHER;
}

void LauncherBullet::Update() {
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

void LauncherBullet::OnCollision(ICollider* other) {
	if (other->GetCategoryName() == ColliderTags::None::own || other->GetCategoryName() == ColliderTags::Boss::own) {
		isAlive_ = false;
		BaseParticles* hitEffect = ParticleManager::GetInstance()->CrateParticle("MissileHit");
		hitEffect->SetPos(transform_->srt_.translate);
		hitEffect->Reset();
	}
}

void LauncherBullet::Reset(const Vector3& _pos, const Vector3& _velocity) {
	transform_->srt_.translate = _pos;
	velocity_ = _velocity * speed_;
}
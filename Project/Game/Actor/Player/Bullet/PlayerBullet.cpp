#include "PlayerBullet.h"
#include "Game/Information/ColliderCategory.h"
#include "Engine/System/Manager/ParticleManager.h"
#include "Engine/System/Manager/GPUParticleManager.h"

PlayerBullet::~PlayerBullet() {
	BaseBullet::Finalize();
	AOENGINE::GpuParticleManager::GetInstance()->DeleteEmitter(trail_);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerBullet::Init() {
	BaseBullet::Init("PlayerBullet");
	object_->SetObject("playerBullet.obj");
	object_->SetCollider(ColliderTags::Bullet::machinegun, ColliderShape::Sphere);

	AOENGINE::BaseCollider* collider = object_->GetCollider(ColliderTags::Bullet::machinegun);
	collider->SetTarget(ColliderTags::Boss::own);
	collider->SetTarget(ColliderTags::Enemy::own);
	collider->SetOnCollision([this](AOENGINE::BaseCollider* other) { OnCollision(other); });
	collider->SetIsTrigger(true);

	type_ = BulletType::Normal;

	object_->SetIsRendering(false);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerBullet::Update() {
	Math::Vector3 pos = transform_->GetTranslate();
	if (std::abs(pos.x) >= deadDistance_) {
		isAlive_ = false;
	}

	if (std::abs(pos.y) >= deadDistance_) {
		isAlive_ = false;
	}

	if (std::abs(pos.z) >= deadDistance_) {
		isAlive_ = false;
	}

	BaseBullet::Update();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 衝突処理
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerBullet::OnCollision(AOENGINE::BaseCollider* other) {
	if (other->GetCategoryName() == ColliderTags::None::own) {
		isAlive_ = false;
		AOENGINE::BaseParticles* hitEffect = AOENGINE::ParticleManager::GetInstance()->CreateParticle("MissileHit");
		hitEffect->SetPos(object_->GetPosition());
		hitEffect->Reset();
	}
}

void PlayerBullet::Reset(const Math::Vector3& pos, const Math::Vector3& velocity) {
	transform_->SetTranslate(pos);
	transform_->Update();
	velocity_ = velocity;

	trail_ = AOENGINE::GpuParticleManager::GetInstance()->CreateEmitter("bulletTrail");
	trail_->SetParent(transform_->GetWorldMatrix());
}
#include "PlayerBullet.h"
#include "Game/Information/ColliderCategory.h"
#include "Engine/System/Manager/ParticleManager.h"

PlayerBullet::~PlayerBullet() {
	BaseBullet::Finalize();
	GpuParticleManager::GetInstance()->DeleteEmitter(trail_);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerBullet::Init() {
	BaseBullet::Init("PlayerBullet");
	object_->SetObject("playerBullet.obj");
	object_->SetCollider(ColliderTags::Bullet::machinegun, ColliderShape::Sphere);

	BaseCollider* collider = object_->GetCollider(ColliderTags::Bullet::machinegun);
	collider->SetTarget(ColliderTags::Boss::own);
	collider->SetOnCollision([this](BaseCollider* other) { OnCollision(other); });
	collider->SetIsTrigger(true);

	type_ = BulletType::Normal;

	object_->SetIsRendering(false);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerBullet::Update() {
	if (std::abs(transform_->srt_.translate.x) >= deadDistance_) {
		isAlive_ = false;
	}

	if (std::abs(transform_->srt_.translate.y) >= deadDistance_) {
		isAlive_ = false;
	}

	if (std::abs(transform_->srt_.translate.z) >= deadDistance_) {
		isAlive_ = false;
	}

	BaseBullet::Update();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 衝突処理
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerBullet::OnCollision(BaseCollider* other) {
	if (other->GetCategoryName() == ColliderTags::None::own) {
		isAlive_ = false;
		BaseParticles* hitEffect = ParticleManager::GetInstance()->CrateParticle("MissileHit");
		hitEffect->SetPos(object_->GetPosition());
		hitEffect->Reset();
	}
}

void PlayerBullet::Reset(const Math::Vector3& pos, const Math::Vector3& velocity) {
	transform_->srt_.translate = pos;
	transform_->Update();
	velocity_ = velocity;

	trail_ = GpuParticleManager::GetInstance()->CreateEmitter("bulletTrail");
	trail_->SetParent(transform_->GetWorldMatrix());
}
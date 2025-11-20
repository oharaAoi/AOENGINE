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

	ICollider* collider = object_->GetCollider(ColliderTags::Bullet::machinegun);
	collider->SetTarget(ColliderTags::Boss::own);
	collider->SetOnCollision([this](ICollider* other) { OnCollision(other); });
	collider->SetIsTrigger(true);

	type_ = BulletType::Normal;

	trail_ = GpuParticleManager::GetInstance()->CreateEmitter("bulletTrail");
	trail_->SetParent(transform_->GetWorldMatrix());

	object_->SetIsRendering(false);

}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerBullet::Update() {
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
// ↓ 衝突処理
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerBullet::OnCollision(ICollider* other) {
	if (other->GetCategoryName() == ColliderTags::None::own) {
		isAlive_ = false;
		BaseParticles* hitEffect = ParticleManager::GetInstance()->CrateParticle("MissileHit");
		hitEffect->SetPos(object_->GetPosition());
		hitEffect->Reset();
	}
}
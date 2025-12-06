#include "BossBullet.h"
#include "Game/Information/ColliderCategory.h"

BossBullet::~BossBullet() {
	BaseBullet::Finalize();
	GpuParticleManager::GetInstance()->DeleteEmitter(trail_);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossBullet::Init() {
	BaseBullet::Init("BossBullet");
	object_->SetObject("playerBullet.obj");
	object_->SetCollider(ColliderTags::Boss::missile, ColliderShape::Sphere);

	AOENGINE::BaseCollider* collider = object_->GetCollider(ColliderTags::Boss::missile);
	collider->SetTarget(ColliderTags::Player::own);
	collider->SetTarget(ColliderTags::None::own);
	collider->SetOnCollision([this](AOENGINE::BaseCollider* other) { OnCollision(other); });
	
	trail_ = GpuParticleManager::GetInstance()->CreateEmitter("bulletTrail");
	trail_->SetParent(transform_->GetWorldMatrix());

	object_->SetIsRendering(false);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossBullet::Update() {
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

void BossBullet::Reset(const Math::Vector3& pos, const Math::Vector3& velocity) {
	transform_->srt_.translate = pos;
	velocity_ = velocity;
}

void BossBullet::OnCollision(AOENGINE::BaseCollider* other) {
	if (other->GetCategoryName() == ColliderTags::None::own) {
		isAlive_ = false;
		BaseParticles* hitEffect = ParticleManager::GetInstance()->CrateParticle("MissileHit");
		hitEffect->SetPos(object_->GetPosition());
		hitEffect->Reset();
	}
}
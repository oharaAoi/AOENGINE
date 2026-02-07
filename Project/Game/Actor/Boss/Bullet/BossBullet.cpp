#include "BossBullet.h"
#include "Game/Information/ColliderCategory.h"
#include "Engine/System/Manager/GpuParticleManager.h"
#include "Engine/System/Manager/ParticleManager.h"

BossBullet::~BossBullet() {
	BaseBullet::Finalize();
	AOENGINE::GpuParticleManager::GetInstance()->DeleteEmitter(trail_);
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
	
	trail_ = AOENGINE::GpuParticleManager::GetInstance()->CreateEmitter("bulletTrail");
	trail_->SetParent(transform_->GetWorldMatrix());

	object_->SetIsRendering(false);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossBullet::Update() {
	Math::QuaternionSRT srt = transform_->GetSRT();
	if (std::abs(srt.translate.x) >= deadLength_) {
		isAlive_ = false;
	}

	if (std::abs(srt.translate.y) >= deadLength_) {
		isAlive_ = false;
	}

	if (std::abs(srt.translate.z) >= deadLength_) {
		isAlive_ = false;
	}

	BaseBullet::Update();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ リセット処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossBullet::Reset(const Math::Vector3& pos, const Math::Vector3& velocity) {
	transform_->SetTranslate(pos);
	velocity_ = velocity;
}

void BossBullet::OnCollision(AOENGINE::BaseCollider* other) {
	if (other->GetCategoryName() == ColliderTags::None::own) {
		isAlive_ = false;
		AOENGINE::BaseParticles* hitEffect = AOENGINE::ParticleManager::GetInstance()->CreateParticle("MissileHit");
		hitEffect->SetPos(object_->GetPosition());
		hitEffect->Reset();
	}
}
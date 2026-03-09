#include "FlamethrowerBullet.h"
#include "Game/Information/ColliderCategory.h"
#include "Engine/Lib/GameTimer.h"

FlamethrowerBullet::~FlamethrowerBullet() {
	BaseBullet::Finalize();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void FlamethrowerBullet::Init() {
	BaseBullet::Init("LauncherBullet");
	object_->SetCollider(ColliderTags::Bullet::flamethrower, ColliderShape::Sphere);

	// ----------------------
	// ↓ colliderの設定
	// ----------------------
	AOENGINE::BaseCollider* collider = object_->GetCollider(ColliderTags::Bullet::flamethrower);
	collider->SetTarget(ColliderTags::Player::own);
	collider->SetTarget(ColliderTags::None::own);
	collider->SetOnCollision([this](AOENGINE::BaseCollider* other) { OnCollision(other); });
	collider->SetIsTrigger(true);

	// timerの設定
	timer_ = AOENGINE::Timer(kLifeTime_);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void FlamethrowerBullet::Update() {
	Math::Vector3 pos = transform_->GetTranslate();
	if (std::abs(pos.x) >= 200.0f) {
		isAlive_ = false;
	}

	if (std::abs(pos.y) >= 200.0f) {
		isAlive_ = false;
	}

	if (std::abs(pos.z) >= 200.0f) {
		isAlive_ = false;
	}

	if(!timer_.Run(AOENGINE::GameTimer::DeltaTime())) {
		isAlive_ = false;
	}

	velocity_ += direction_ * speed_ * AOENGINE::GameTimer::DeltaTime();
	BaseBullet::Update();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 情報を再設定する処理
///////////////////////////////////////////////////////////////////////////////////////////////

void FlamethrowerBullet::Reset(const Math::Vector3& pos, const Math::Vector3& direction,
							   float speed, float radius) {
	transform_->SetTranslate(pos);
	direction_ = direction;
	speed_ = speed;
	AOENGINE::BaseCollider* collider = object_->GetCollider(ColliderTags::Bullet::flamethrower);
	collider->SetRadius(radius);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 衝突処理
///////////////////////////////////////////////////////////////////////////////////////////////

void FlamethrowerBullet::OnCollision(AOENGINE::BaseCollider* other) {
	if (other->GetCategoryName() == ColliderTags::None::own || other->GetCategoryName() == ColliderTags::Player::own) {
		isAlive_ = false;
	}
}
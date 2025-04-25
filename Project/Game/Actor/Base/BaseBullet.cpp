#include "BaseBullet.h"

void BaseBullet::Finalize() {
	BaseGameObject::Finalize();
}

void BaseBullet::Init() {
	BaseGameObject::Init();
	isAlive_ = true;
}

void BaseBullet::Update() {
	transform_->translate_ += velocity_ * GameTimer::DeltaTime();
	BaseGameObject::Update();
}

void BaseBullet::Draw() const {
	BaseGameObject::Draw();
}

void BaseBullet::Reset(const Vector3& pos, const Vector3& velocity) {
	transform_->translate_ = pos;
	velocity_ = velocity;
}

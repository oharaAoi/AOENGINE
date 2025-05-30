#include "BaseBullet.h"
#include "Engine/Render/SceneRenderer.h"

void BaseBullet::Finalize() {
	BaseGameObject::Finalize();
}

void BaseBullet::Init() {
	BaseGameObject::Init();
	isAlive_ = true;

	SceneRenderer::GetInstance()->SetObject(Object3dPSO::Normal, this);
}

void BaseBullet::Update() {
	if (!isAlive_) { return; };
	transform_->translate_ += velocity_ * GameTimer::DeltaTime();
	if (velocity_.x != 0.0f || velocity_.y != 0.0f) {
		transform_->rotation_ = Quaternion::LookRotation(velocity_.Normalize());
	}
	BaseGameObject::Update();
}

void BaseBullet::Draw() const {
	if (!isAlive_) { return; };
	BaseGameObject::Draw();
}

void BaseBullet::Reset(const Vector3& pos, const Vector3& velocity) {
	transform_->translate_ = pos;
	velocity_ = velocity;
}

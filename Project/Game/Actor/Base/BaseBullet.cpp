#include "BaseBullet.h"
#include "Engine/Render/SceneRenderer.h"

void BaseBullet::Finalize() {
	object_->SetIsDestroy(true);
}

void BaseBullet::Init(const std::string& bulletName) {
	isAlive_ = true;

	object_ = SceneRenderer::GetInstance()->AddObject<BaseGameObject>(bulletName, "Object_Normal.json");
	transform_ = object_->GetTransform();
}

void BaseBullet::Update() {
	if (!isAlive_) { return; };
	transform_->srt_.translate += velocity_ * GameTimer::DeltaTime();
	if (velocity_.x != 0.0f || velocity_.y != 0.0f) {
		transform_->srt_.rotate = Quaternion::LookRotation(velocity_.Normalize());
	}
}

void BaseBullet::Reset(const Vector3& pos, const Vector3& velocity) {
	transform_->srt_.translate = pos;
	velocity_ = velocity;
}

void BaseBullet::Debug_Gui() {
}
#include "BaseBullet.h"
#include "Engine/Render/SceneRenderer.h"
#include "Engine/Lib/GameTimer.h"

void BaseBullet::Finalize() {
	object_->SetIsDestroy(true);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BaseBullet::Init(const std::string& bulletName) {
	isAlive_ = true;

	object_ = AOENGINE::SceneRenderer::GetInstance()->AddObject<AOENGINE::BaseGameObject>(bulletName, "Object_Normal.json");
	transform_ = object_->GetTransform();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BaseBullet::Update() {
	if (!isAlive_) { return; };
	transform_->srt_.translate += velocity_ * AOENGINE::GameTimer::DeltaTime();
	if (velocity_.x != 0.0f || velocity_.y != 0.0f) {
		transform_->srt_.rotate = Math::Quaternion::LookRotation(velocity_.Normalize());
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ リセット処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BaseBullet::Reset(const Math::Vector3& pos, const Math::Vector3& velocity) {
	transform_->srt_.translate = pos;
	velocity_ = velocity;
}

void BaseBullet::Debug_Gui() {
	object_->Debug_Gui();
}
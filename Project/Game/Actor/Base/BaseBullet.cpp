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
	Math::QuaternionSRT srt = transform_->GetSRT();
	srt.translate += velocity_ * AOENGINE::GameTimer::DeltaTime();
	if (velocity_.x != 0.0f || velocity_.y != 0.0f) {
		srt.rotate = Math::Quaternion::LookRotation(velocity_.Normalize());
	}
	transform_->SetSRT(srt);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ リセット処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BaseBullet::Reset(const Math::Vector3& pos, const Math::Vector3& velocity) {
	Math::QuaternionSRT srt = transform_->GetSRT();
	srt.translate = pos;
	velocity_ = velocity;
	transform_->SetSRT(srt);
}

void BaseBullet::Debug_Gui() {
	object_->Debug_Gui();
}
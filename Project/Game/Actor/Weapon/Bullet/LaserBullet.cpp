#include "LaserBullet.h"
#include "Engine/Engine.h"
#include "Engine/Render/SceneRenderer.h"
#include "Game/Information/ColliderCategory.h"

LaserBullet::~LaserBullet() {
	BaseBullet::Finalize();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集処理
///////////////////////////////////////////////////////////////////////////////////////////////

void LaserBullet::Debug_Gui() {
	param_.Debug_Gui();
	parentTransform_->Debug_Gui();
	BaseBullet::Debug_Gui();
	ImGui::DragFloat3("targetPos", &targetPos_.x);
	if (ImGui::Button("shot")) {
		isShot_ = true;
		parentTransform_->SetScale(Vector3(1,1,0.0f));

		Reset(CVector3::ZERO, targetPos_, 200.0f);
	}
}

void LaserBullet::LaserParameter::Debug_Gui() {
	ImGui::DragFloat("maxLength", &maxLength);
	SaveAndLoad();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void LaserBullet::Init() {
	BaseBullet::Init("LaserBullet");
	object_->SetObject("laser.obj");
	object_->SetTexture("laser.png");
	object_->SetIsLighting(false);
	object_->SetIsShadow(false);
	ICollider* collider = object_->SetCollider(ColliderTags::Bullet::laser, ColliderShape::Line);
	collider->SetTarget(ColliderTags::Boss::own);
	collider->SetTarget(ColliderTags::Field::ground);
	collider->SetTarget(ColliderTags::None::own);
	collider->SetIsTrigger(true);
	collider->SetOnCollision([this](ICollider* other) { OnCollision(other); });
	lineCollider_ = dynamic_cast<LineCollider*>(collider);

	type_ = BulletType::Laser;

	parentTransform_ = Engine::CreateWorldTransform();

	param_.Load();

	transform_->SetTranslationZ(1.0f);
	transform_->SetParent(parentTransform_->GetWorldMatrix());

	isShot_ = true;

	SceneRenderer::GetInstance()->ChangeRenderingType("Object_laser.json", object_);
	//EditorWindows::AddObjectWindow(this, "Laser");
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void LaserBullet::Update() {
	// scaleを大きくする
	if (isShot_) {
		Vector3 scale = parentTransform_->GetScale();
		scale.z += speed_ * GameTimer::DeltaTime();
		parentTransform_->SetScale(scale);

		if (scale.z >= param_.maxLength) {
			isShot_ = false;
			isAlive_ = false;
		}

		Vector3 diff = dire_ * scale.z;
		lineCollider_->SetDiff(diff);
	}

	lineCollider_->Update(QuaternionSRT(CVector3::ZERO, Quaternion(), parentTransform_->GetTranslate()));
	parentTransform_->Update();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ リセット処理
///////////////////////////////////////////////////////////////////////////////////////////////

void LaserBullet::Reset(const Vector3& _pos, const Vector3& _targetPos, float _speed) {
	parentTransform_->SetTranslate(_pos);
	dire_ = Vector3(_targetPos - _pos).Normalize();
	parentTransform_->SetRotate(Quaternion::LookRotation(dire_));
	speed_ = _speed;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 衝突処理
///////////////////////////////////////////////////////////////////////////////////////////////

void LaserBullet::OnCollision(ICollider* _other) {
	if (_other->GetCategoryName() == ColliderTags::None::own || _other->GetCategoryName() == ColliderTags::Boss::own) {
		isAlive_ = false;
	}
}
#include "LaserBullet.h"
#include "Engine/Engine.h"
#include "Engine/Render/SceneRenderer.h"
#include "Engine/System/Manager/ParticleManager.h"
#include "Game/Information/ColliderCategory.h"

LaserBullet::~LaserBullet() {
	BaseBullet::Finalize();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集処理
///////////////////////////////////////////////////////////////////////////////////////////////

void LaserBullet::Debug_Gui() {
	isFade_ = false;

	param_.Debug_Gui();
	parentTransform_->Debug_Gui();
	BaseBullet::Debug_Gui();
	ImGui::DragFloat3("targetPos", &targetPos_.x);
	if (ImGui::Button("shot")) {
		isShot_ = true;
		parentTransform_->SetScale(Vector3(1, 1, 0.0f));

		Reset(CVector3::ZERO, targetPos_, 200.0f);
	}
}

void LaserBullet::LaserParameter::Debug_Gui() {
	ImGui::DragFloat("maxLength", &maxLength);
	ImGui::DragFloat("fadeTime", &fadeTime);
	SaveAndLoad();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void LaserBullet::Init() {
	BaseBullet::Init("LaserBullet");
	type_ = BulletType::Laser;
	param_.Load();

	// ----------------------
	// ↓ objectの設定
	// ----------------------
	object_->SetObject("laser.obj");
	object_->SetTexture("laser.png");
	object_->SetIsLighting(false);
	object_->SetIsShadow(false);

	// ----------------------
	// ↓ colliderの設定
	// ----------------------
	ICollider* collider = object_->SetCollider(ColliderTags::Bullet::laser, ColliderShape::Line);
	collider->SetTarget(ColliderTags::Boss::own);
	collider->SetTarget(ColliderTags::Field::ground);
	collider->SetTarget(ColliderTags::None::own);
	collider->SetIsTrigger(true);
	collider->SetOnCollision([this](ICollider* other) { OnCollision(other); });
	lineCollider_ = dynamic_cast<LineCollider*>(collider);

	// ----------------------
	// ↓ Parameter系の設定
	// ----------------------
	parentTransform_ = Engine::CreateWorldTransform();

	transform_->SetTranslationZ(1.0f);
	transform_->SetParent(parentTransform_->GetWorldMatrix());

	isShot_ = true;
	isFade_ = false;

	// ----------------------
	// ↓ Effectの設定
	// ----------------------
	laserCylinder_ = std::make_unique<LaserCylinder>();
	laserCylinder_->Init();
	laserCylinder_->GetTransform()->SetParent(parentTransform_->GetWorldMatrix());
	AddChild(laserCylinder_.get());

	ParticleManager* manager = ParticleManager::GetInstance();
	shotEffect_ = manager->CrateParticle("laserShot");

	fadeTimer_ = Timer(param_.fadeTime);

	SceneRenderer::GetInstance()->ChangeRenderingType("Object_laser.json", object_);
	EditorWindows::AddObjectWindow(this, "Laser");
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void LaserBullet::Update() {

	Stretch();

	Fade();

	laserCylinder_->Update();
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
	shotEffect_->SetPos(_pos);
	shotEffect_->Reset();

	isShot_ = true;
	isFade_ = false;

	fadeTimer_ = Timer(param_.fadeTime);
}

void LaserBullet::Stretch() {
	// scaleを大きくする
	if (isShot_) {
		Vector3 scale = parentTransform_->GetScale();
		scale.z += speed_ * GameTimer::DeltaTime();
		parentTransform_->SetScale(scale);

		if (scale.z >= param_.maxLength) {
			isShot_ = false;
			isFade_ = true;
		}

		Vector3 diff = dire_ * scale.z;
		lineCollider_->SetDiff(diff);
		laserCylinder_->SetUvScale(scale.z);
	}
}

void LaserBullet::Fade() {
	if (isFade_) {
		if (fadeTimer_.Run(GameTimer::DeltaTime())) {
			float z = std::lerp(1.f, 0.f, fadeTimer_.t_);
			parentTransform_->SetScaleX(z);
			parentTransform_->SetScaleY(z);

			transform_->SetScaleZ(z);
			laserCylinder_->SetScaleZ(z);

			object_->SetColor(Color(1, 1, 1, z));
			laserCylinder_->GetGameObject()->SetColor(Color(1, 1, 1, z));

		} else {
			isAlive_ = false;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 衝突処理
///////////////////////////////////////////////////////////////////////////////////////////////

void LaserBullet::OnCollision(ICollider* _other) {
	if (_other->GetCategoryName() == ColliderTags::None::own || _other->GetCategoryName() == ColliderTags::Boss::own) {
		isFade_ = true;
		isShot_ = false;
	}
}
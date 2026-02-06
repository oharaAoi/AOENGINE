#include "LauncherBullet.h"
#include "Engine/System/Audio/AudioPlayer.h"
#include "Game/Information/ColliderCategory.h"
#include "Engine/System/Manager/ParticleManager.h"

LauncherBullet::~LauncherBullet() {
	BaseBullet::Finalize();
	AOENGINE::ParticleManager::GetInstance()->DeleteParticles(burn_);
	AOENGINE::ParticleManager::GetInstance()->DeleteParticles(smoke_);
	burn_ = nullptr;
	smoke_ = nullptr;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void LauncherBullet::Init() {
	BaseBullet::Init("LauncherBullet");
	object_->SetObject("missile.obj");
	object_->SetCollider(ColliderTags::Bullet::rocket, ColliderShape::Sphere);

	// ----------------------
	// ↓ colliderの設定
	// ----------------------
	AOENGINE::BaseCollider* collider = object_->GetCollider(ColliderTags::Bullet::rocket);
	collider->SetTarget(ColliderTags::Boss::own);
	collider->SetTarget(ColliderTags::Enemy::own);
	collider->SetTarget(ColliderTags::Field::ground);
	collider->SetTarget(ColliderTags::None::own);
	collider->SetOnCollision([this](AOENGINE::BaseCollider* other) { OnCollision(other); });
	collider->SetIsTrigger(true);

	// ----------------------
	// ↓ effectの初期化
	// ----------------------
	burn_ = AOENGINE::ParticleManager::GetInstance()->CreateParticle("MissileBurn");
	smoke_ = AOENGINE::ParticleManager::GetInstance()->CreateParticle("Launcher");
	burn_->SetParent(transform_->GetWorldMatrix());
	smoke_->SetParent(transform_->GetWorldMatrix());
	burn_->SetIsStop(false);
	smoke_->SetIsStop(false);

	type_ = BulletType::Launcher;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void LauncherBullet::Update() {
	if (std::abs(transform_->srt_.translate.x) >= 200.0f) {
		isAlive_ = false;
	}

	if (std::abs(transform_->srt_.translate.y) >= 200.0f) {
		isAlive_ = false;
	}

	if (std::abs(transform_->srt_.translate.z) >= 200.0f) {
		isAlive_ = false;
	}

	BaseBullet::Update();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 衝突時の処理
///////////////////////////////////////////////////////////////////////////////////////////////

void LauncherBullet::OnCollision(AOENGINE::BaseCollider* other) {
	if (other->GetCategoryName() == ColliderTags::None::own || other->GetCategoryName() == ColliderTags::Boss::own) {
		isAlive_ = false;
		AOENGINE::BaseParticles* hitEffect = AOENGINE::ParticleManager::GetInstance()->CreateParticle("Expload");
		hitEffect->SetPos(transform_->srt_.translate);
		hitEffect->Reset();

		AOENGINE::AudioPlayer::SingleShotPlay("luncherHit.mp3", param_.hitSeVolume);
	}
}

void LauncherBullet::Reset(const Math::Vector3& _pos, const Math::Vector3& _velocity) {
	transform_->srt_.translate = _pos;
	velocity_ = _velocity;
}

void LauncherBullet::BulletParam::Debug_Gui() {
	ImGui::DragFloat("hitSeVolume", &hitSeVolume, 0.1f);
}

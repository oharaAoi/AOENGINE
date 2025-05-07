#include "PBulletToBossCallBacks.h"
#include "Engine/Render.h"
#include "Game/Information/ColliderCategory.h"

void PBulletToBossCallBacks::Init() {
	SetCallBacks();
	SetPair(pCollisionManager_, ColliderTags::Bullet::machinegun, ColliderTags::Boss::own);

	hitBossSmoke_ = std::make_unique<HitBossSmoke>();
	hitBossSmoke_->Init("HitBossSmoke");

	hitBossSmokeBorn_ = std::make_unique<HitBossSmokeBorn>();
	hitBossSmokeBorn_->Init("HitBossSmokeBorn");

	hitBossExploadParticles_ = std::make_unique<HitBossExploadParticles>();
	hitBossExploadParticles_->Init("HitBossExploadParticles");
}

void PBulletToBossCallBacks::Update() {
	hitBossSmoke_->Update(Render::GetCameraRotate());
	hitBossSmokeBorn_->Update(Render::GetCameraRotate());
	hitBossExploadParticles_->Update(Render::GetCameraRotate());
}

void PBulletToBossCallBacks::CollisionEnter([[maybe_unused]] ICollider* const bullet, [[maybe_unused]] ICollider* const boss) {
	hitBossExploadParticles_->SetPos(bullet->GetCenterPos());
	hitBossExploadParticles_->SetOnShot();

	hitBossSmoke_->SetPos(bullet->GetCenterPos());
	hitBossSmoke_->SetOnShot();

	hitBossSmokeBorn_->SetPos(bullet->GetCenterPos());
	hitBossSmokeBorn_->SetOnShot();
}

void PBulletToBossCallBacks::CollisionStay([[maybe_unused]] ICollider* const bullet, [[maybe_unused]] ICollider* const boss) {
}

void PBulletToBossCallBacks::CollisionExit([[maybe_unused]] ICollider* const bullet, [[maybe_unused]] ICollider* const boss) {
}

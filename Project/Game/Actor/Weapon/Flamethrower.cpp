#include "Flamethrower.h"
#include "Engine/System/Manager/ParticleManager.h"
#include "Engine/Lib/GameTimer.h"
#include "Game/Information/ColliderCategory.h"
#include "Game/Actor/Weapon/Bullet/FlamethrowerBullet.h"

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集処理
///////////////////////////////////////////////////////////////////////////////////////////////

void Flamethrower::Debug_Gui() {
	flamethrowerParam_.Debug_Gui();

	ImGui::Separator();
	if (ImGui::Button("Togle")) {
		bool isFlag = flameParticle_->GetIsStop();
		isFlag = !isFlag;
		flameParticle_->SetIsStop(isFlag);
	}

	transform_->SetTranslate(flamethrowerParam_.pos);
}

void Flamethrower::FlamethrowerParam::Debug_Gui() {
	ImGui::DragFloat3("pos", &pos.x, 0.1f);
	ImGui::DragFloat("bulletSpeed", &bulletSpeed, 0.1f);
	ImGui::DragFloat("bulletRadius", &bulletRadius, 0.1f);
	ImGui::DragFloat("bulletDamage", &bulletDamage, 0.1f);
	ImGui::DragFloat("shotInterval", &shotInterval, 0.1f);
	SaveAndLoad();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 終了処理
///////////////////////////////////////////////////////////////////////////////////////////////

void Flamethrower::Finalize() {
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void Flamethrower::Init() {
	SetName("Flamethrower");
	BaseWeapon::Init();

	attackParam_.SetName("FlamethrowerAttackParam");
	attackParam_.Load();
	flamethrowerParam_.Load();

	object_->SetObject("flamethrower.obj", MaterialType::PBR);
	transform_->SetTranslate(flamethrowerParam_.pos);

	// ----------------------
	// ↓ colliderの設定
	// ----------------------
	collider_ = object_->SetCollider(ColliderTags::Bullet::flamethrower, ColliderShape::Sphere);
	collider_->SetTarget(ColliderTags::Player::own);

	// ----------------------
	// ↓火炎の表現のための初期化
	// ----------------------
	flameParticle_ = AOENGINE::ParticleManager::GetInstance()->CreateParticle("fire");
	flameParticle_->SetIsStop(false);
	flameParticle_->SetParent(transform_);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 攻撃処理
///////////////////////////////////////////////////////////////////////////////////////////////

bool Flamethrower::Attack([[maybe_unused]] const AttackContext& cxt) {
	// 攻撃の時間を管理しておく
	attackTimer_ += AOENGINE::GameTimer::DeltaTime();
	if (flamethrowerParam_.shotInterval < attackTimer_) {
		isCanAttack_ = true;
		attackTimer_ = 0.f;
	} else {
		isCanAttack_ = false;
	}

	// 攻撃可能かどうか
	if (!isCanAttack_) { return false; }

	Math::QuaternionSRT worldSrt = transform_->GetWorldSRT();
	Math::Vector3 pos = worldSrt.translate;
	Math::Vector3 direction = -worldSrt.rotate.MakeForward();

	FlamethrowerBullet* bullet = pBulletManager_->AddBullet<FlamethrowerBullet>(pos, direction,
																				flamethrowerParam_.bulletSpeed, flamethrowerParam_.bulletRadius);
	bullet->SetTakeDamage(flamethrowerParam_.bulletDamage);

	return true;
}
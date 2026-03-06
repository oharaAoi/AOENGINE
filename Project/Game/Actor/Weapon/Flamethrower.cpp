#include "Flamethrower.h"
#include "Engine/System/Manager/ParticleManager.h"
#include "Game/Information/ColliderCategory.h"

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集処理
///////////////////////////////////////////////////////////////////////////////////////////////

void Flamethrower::Debug_Gui() {
	BaseWeapon::Debug_Gui();
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
	flameParticle_->SetIsStop(true);
	flameParticle_->SetParent(transform_);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 攻撃処理
///////////////////////////////////////////////////////////////////////////////////////////////

bool Flamethrower::Attack(const AttackContext& cxt) {
	if (!isCanAttack_) { return false; }

	return true;
}
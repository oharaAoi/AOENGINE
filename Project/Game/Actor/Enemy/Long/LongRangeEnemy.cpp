#include "LongRangeEnemy.h"
#include "Engine/Render/SceneRenderer.h"
#include "Game/Information/ColliderCategory.h"

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void LongRangeEnemy::Init() {
	SetName("LongRangeEnemy");

	// ----------------------
	// ↓ objectをシーンに追加する
	// ----------------------
	object_ = AOENGINE::SceneRenderer::GetInstance()->AddObject<AOENGINE::BaseGameObject>("LongRangeEnemy", "Object_PBR.json", 100);
	transform_ = object_->GetTransform();
	object_->SetObject("enemy.gltf", MaterialType::PBR);

	// ----------------------
	// ↓ アニメションの追加
	// ----------------------
	object_->SetAnimator("./Project/Packages/Game/Assets/Load/Models/Enemy/", "enemy.gltf", true, true, false);
	object_->GetAnimetor()->GetAnimationClip()->PoseToAnimation("pause", 0.2f);

	// ----------------------
	// ↓ Colliderの追加
	// ----------------------
	AOENGINE::BaseCollider* collider = object_->SetCollider(ColliderTags::Enemy::own, ColliderShape::Sphere);
	collider->SetIsStatic(false);
	
	// ----------------------
	// ↓ 物理の挙動を追加
	// ----------------------
	object_->SetPhysics();
	object_->GetRigidbody()->SetGravity(false);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void LongRangeEnemy::Update() {
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集処理
///////////////////////////////////////////////////////////////////////////////////////////////

void LongRangeEnemy::Debug_Gui() {
}

#include "ShortRangeEnemy.h"
#include "Engine/Render/SceneRenderer.h"
#include "Game/Information/ColliderCategory.h"

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void ShortRangeEnemy::Init() {
	SetName("ShortRangeEnemy");

	object_ = AOENGINE::SceneRenderer::GetInstance()->AddObject<AOENGINE::BaseGameObject>("LongRangeEnemy", "Object_PBR.json", 100);
	transform_ = object_->GetTransform();
	object_->SetObject("enemy.gltf", MaterialType::PBR);

	// ----------------------
	// ↓ アニメションの追加
	// ----------------------
	object_->SetAnimator("./Project/Packages/Game/Assets/Load/Models/Enemy/", "enemy.gltf", true, true, false);
	object_->GetAnimator()->GetAnimationClip()->PoseToAnimation("pause");

	// ----------------------
	// ↓ Colliderの追加
	// ----------------------
	AOENGINE::BaseCollider* collider = object_->SetCollider(ColliderTags::Enemy::own, ColliderShape::Sphere);
	collider->SetIsStatic(false);
	//collider->SetLocalPos(param_.colliderLocalPos);

	// ----------------------
	// ↓ 物理の挙動を追加
	// ----------------------
	object_->SetPhysics();
	object_->GetRigidbody()->SetGravity(false);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void ShortRangeEnemy::Update() {
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集処理
///////////////////////////////////////////////////////////////////////////////////////////////

void ShortRangeEnemy::Debug_Gui() {
}

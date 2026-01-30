#include "LongRangeEnemy.h"
#include "Engine/Render/SceneRenderer.h"
#include "Game/Information/ColliderCategory.h"

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void LongRangeEnemy::Init() {
	SetName("LongRangeEnemy");
	param_.Load();

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
	object_->GetAnimator()->GetAnimationClip()->PoseToAnimation("pause", 0.2f);

	// ----------------------
	// ↓ Colliderの追加
	// ----------------------
	AOENGINE::BaseCollider* collider = object_->SetCollider(ColliderTags::Enemy::own, ColliderShape::Sphere);
	collider->SetIsStatic(false);
	collider->SetLocalPos(param_.colliderLocalPos);
	
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
	param_.Debug_Gui();

	AOENGINE::BaseCollider* collider = object_->GetCollider(ColliderTags::Enemy::own);
	collider->SetRadius(param_.colliderRadius);
	collider->SetLocalPos(param_.colliderLocalPos);
}

void LongRangeEnemy::Parameter::Debug_Gui() {
	ImGui::DragFloat("コライダーの大きさ", &colliderRadius, 0.1f);
	ImGui::DragFloat3("コライダーのローカル座標", &colliderLocalPos.x, 0.1f);
	SaveAndLoad();
}
#include "BaseEnemy.h"
#include "Engine/Render/SceneRenderer.h"
#include "Game/Information/ColliderCategory.h"
#include "Engine/System/Editor/Window/EditorWindows.h"

BaseEnemy::BaseEnemy() {
	isDead_ = false;
}

BaseEnemy::~BaseEnemy() {
	AOENGINE::SceneRenderer::GetInstance()->ReleaseObject(object_);
	object_ = nullptr;
#ifdef _DEBUG
	AOENGINE::EditorWindows::GetInstance()->GetObjectWindow()->DeleteObject(object_);
#endif // _DEBUG

}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集
///////////////////////////////////////////////////////////////////////////////////////////////

void BaseEnemy::Debug_Gui() {
	ImGui::Checkbox("カメラに写っているかどうか", &isWithinSight_);
	object_->Debug_Gui();
	ImGui::Separator();
	baseParam_.Debug_Gui();
}

void BaseEnemy::BaseParameter::Debug_Gui() {
	ImGui::DragFloat("hp", &health, 0.1f);
	ImGui::DragFloat("耐久度", &postureStability, 0.1f);
	ImGui::DragFloat("耐久度を削る割合", &postureStabilityScrapeRaito, 0.1f);
	ImGui::DragFloat("angularVelocity", &angularVelocity, 0.1f);
	ImGui::DragFloat("angularThreshold", &angularThreshold, 0.1f);
	SaveAndLoad();
}

void BaseEnemy::InitCommon() {
	transform_ = object_->GetTransform();

	// ----------------------
	// ↓ アニメションの追加
	// ----------------------
	object_->SetObject("enemy.gltf", MaterialType::PBR);
	object_->SetAnimator("./Project/Packages/Game/Assets/Load/Models/Enemy/", "enemy.gltf", true, true, false);
	object_->GetAnimator()->GetAnimationClip()->PoseToAnimation("pause");

	// ----------------------
	// ↓ 物理の挙動を追加
	// ----------------------
	object_->SetPhysics();
	object_->GetRigidbody()->SetGravity(false);

	// ----------------------
	// ↓ bornの行列を取得
	// ----------------------
	AOENGINE::Skeleton* skeleton = object_->GetAnimator()->GetSkeleton();
	rightHandMatrix_ = skeleton->GetSkeletonSpaceMat("right_hand") * transform_->GetWorldMatrix();

	// ----------------------
	// ↓ searchColliderの設定
	// ----------------------

	searchCollider_ = object_->SetCollider(ColliderTags::Enemy::searchCollider, ColliderShape::Sphere);
	searchCollider_->SetIsTrigger(true);
}

bool BaseEnemy::TargetLook() {
	Math::Vector3 distance = targetTransform_->GetPos() - transform_->GetPos();
	distance.y = 0.0f;
	// 目標の方向を計算する
	Math::Quaternion targetRotate = Math::Quaternion::LookRotation(distance);
	// なす角を求める
	float angle = Math::Quaternion::Angle(transform_->GetRotate(), targetRotate);

	if (angle > baseParam_.angularThreshold) {
		// 経過時間で変化させる角度
		float deltaAngle = baseParam_.angularVelocity * AOENGINE::GameTimer::DeltaTime();
		// 実際に方向を決定
		transform_->SetRotate(Math::Quaternion::Slerp(transform_->GetRotate(), targetRotate, deltaAngle));
	} else {
		return true;
	}

	return false;
}

void BaseEnemy::Damage(float takeDamage) {
	baseParam_.health -= takeDamage;
	if (baseParam_.health <= 0) {
		isDead_ = true;
	}
}
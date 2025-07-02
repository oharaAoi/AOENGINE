#include "SphereCollider.h"
#include "Engine/Utilities/DrawUtils.h"
#include "Engine/Render.h"
#include "Engine/System/Manager/CollisionLayerManager.h"
#include <assert.h>

SphereCollider::SphereCollider() {}
SphereCollider::~SphereCollider() {}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　初期化処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void SphereCollider::Init(const std::string& categoryName, ColliderShape shape) {
	auto& layers = CollisionLayerManager::GetInstance();
	categoryBits_ = layers.RegisterCategory(categoryName);
	categoryName_ = categoryName;

	collisionPartnersMap_.clear();
	collisionState_ = CollisionFlags::NONE;

	if (shape == ColliderShape::SPHERE) {
		shape_ = Sphere{ .center = CVector3::ZERO, .radius = 1.0f };
	} else {
		assert("not Sphere Shape");
	}

	isActive_ = true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　更新処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void SphereCollider::Update(const QuaternionSRT& srt) {
	pushbackDire_ = CVector3::ZERO;
	centerPos_ = srt.translate + localSRT_.translate;
	std::get<Sphere>(shape_).center = centerPos_;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　描画処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void SphereCollider::Draw() const {
	if (collisionState_ == CollisionFlags::ENTER || collisionState_ == CollisionFlags::STAY) {
		DrawSphere(std::get<Sphere>(shape_).center, std::get<Sphere>(shape_).radius, Render::GetViewProjectionMat(), Vector4(1, 0, 0, 1));
	} else {
		DrawSphere(std::get<Sphere>(shape_).center, std::get<Sphere>(shape_).radius, Render::GetViewProjectionMat(), Vector4(0, 1, 1, 1));
	}
}

void SphereCollider::Debug_Gui() {
	ImGui::DragFloat3("translate", &localSRT_.translate.x, 0.1f);
	ImGui::DragFloat("radius", &std::get<Sphere>(shape_).radius, 0.1f);
}
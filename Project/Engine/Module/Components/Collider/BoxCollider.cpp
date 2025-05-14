#include "BoxCollider.h"
#include "Engine/Utilities/DrawUtils.h"
#include "Engine/Render.h"
#include "Engine/System/Manager/CollisionLayerManager.h"
#include <assert.h>

BoxCollider::BoxCollider() {}
BoxCollider::~BoxCollider() {}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　初期化処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void BoxCollider::Init(const std::string& categoryName, ColliderShape shape) {
	auto& layers = CollisionLayerManager::GetInstance();
	categoryBits_ = layers.RegisterCategory(categoryName);

	collisionState_ = CollisionFlags::NONE;
	
	if (shape == ColliderShape::AABB) {
		shape_ = AABB{ .min = CVector3::UNIT * -1.0f, .max = CVector3::UNIT };
	} else if(shape == ColliderShape::OBB) {
		shape_ = OBB{ .center = CVector3::ZERO, .size = CVector3::UNIT };
	} else {
		assert("not AABB or OBB Shape");
	}

	isActive_ = true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　更新処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void BoxCollider::Update(const QuaternionSRT& srt) {
	centerPos_ = srt.translate;
	if (std::holds_alternative<AABB>(shape_)) {
		std::get<AABB>(shape_).min = (srt.translate - (size_ / 2.0f)) + localSRT_.translate;
		std::get<AABB>(shape_).max = (srt.translate + (size_ / 2.0f)) + localSRT_.translate;
	} else if (std::holds_alternative<OBB>(shape_)) {
		std::get<OBB>(shape_).center = srt.translate + localSRT_.translate;
		std::get<OBB>(shape_).MakeOBBAxis(srt.rotate);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　描画処理
//////////////////////////////////////////////////////////////////////////////////////////////////
void BoxCollider::Draw() const {
	Vector4 color{ 1,1,1,1 };
	if (collisionState_ == CollisionFlags::ENTER || collisionState_ == CollisionFlags::STAY) {
		color = { 1,0,0,1 };
	}

	if (std::holds_alternative<AABB>(shape_)) {
		DrawAABB(std::get<AABB>(shape_), Render::GetViewProjectionMat(), color);
	} else if (std::holds_alternative<OBB>(shape_)) {
		DrawOBB(std::get<OBB>(shape_), Render::GetViewProjectionMat(), color);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　押し戻し処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void BoxCollider::PushBack() {
	if (std::holds_alternative<AABB>(shape_)) {
		std::get<AABB>(shape_).min += pushbackDire_; 
		std::get<AABB>(shape_).max += pushbackDire_;
	} else if (std::holds_alternative<OBB>(shape_)) {
		std::get<OBB>(shape_).center += pushbackDire_;
	}
}

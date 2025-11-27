#include "LineCollider.h"
#include "Engine/Utilities/DrawUtils.h"
#include "Engine/Render.h"
#include "Engine/System/Manager/CollisionLayerManager.h"

void LineCollider::Init(const std::string& categoryName, ColliderShape shape) {
	auto& layers = CollisionLayerManager::GetInstance();
	categoryBits_ = layers.RegisterCategory(categoryName);
	categoryName_ = categoryName;

	collisionPartnersMap_.clear();
	collisionState_ = (int)CollisionFlags::None;

	if (shape == ColliderShape::Line) {
		shape_ = Line{ .origin = CVector3::ZERO, .diff = CVector3::ZERO };
	} else {
		assert("not Line Shape");
	}

	isActive_ = true;
}

void LineCollider::Update(const QuaternionSRT& srt) {
	std::get<Line>(shape_).origin = srt.translate;
}

void LineCollider::Draw() const {
}

void LineCollider::Debug_Gui() {
}

void LineCollider::SetDiff(const Vector3& _diff) {
	std::get<Line>(shape_).diff = _diff;
}

const Vector3& LineCollider::GetDiff() const {
	return std::get<Line>(shape_).diff;
}

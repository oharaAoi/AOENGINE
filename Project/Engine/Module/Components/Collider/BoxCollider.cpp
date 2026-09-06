#include "BoxCollider.h"
#include "Engine/Utilities/DrawUtils.h"
#include "Engine/Render/Render.h"
#include "Engine/System/Manager/CollisionLayerManager.h"
#include <assert.h>
#include <cmath>

using namespace AOENGINE;

BoxCollider::BoxCollider() {}
BoxCollider::~BoxCollider() {}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　初期化処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void BoxCollider::Init(const std::string& categoryName, ColliderShape shape) {
	auto& layers = AOENGINE::CollisionLayerManager::GetInstance();
	layerBit_ = layers.RegisterCategory(categoryName);
	categoryName_ = categoryName;

	collisionPartnersMap_.clear();
	collisionState_ = (int)CollisionFlags::None;

	if (shape == ColliderShape::AABB) {
		shape_ = Math::AABB{ .min = CVector3::UNIT * -1.0f, .max = CVector3::UNIT };
	} else if (shape == ColliderShape::OBB) {
		shape_ = Math::OBB{ .center = CVector3::ZERO, .size = CVector3::UNIT };
	} else {
		assert("not AABB or OBB Shape");
	}

	isActive_ = true;
	size_ = CVector3::UNIT;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　更新処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void BoxCollider::Update(const Math::QuaternionSRT& srt) {
	pushbackDire_ = CVector3::ZERO;
	centerPos_ = srt.translate;
	if (std::holds_alternative<Math::AABB>(shape_)) {
		// AABBの中心と各軸方向の半サイズから、8頂点を生成せずに
		// 回転後の外接AABBを求める。
		const Math::Vector3 halfSize = size_ * 0.5f;
		const Math::Vector3 scaledLocalCenter = localSRT_.translate * srt.scale;
		const Math::Vector3 center = srt.translate + (srt.rotate * scaledLocalCenter);
		const Math::Vector3 axisX = srt.rotate * Math::Vector3(halfSize.x * srt.scale.x, 0.0f, 0.0f);
		const Math::Vector3 axisY = srt.rotate * Math::Vector3(0.0f, halfSize.y * srt.scale.y, 0.0f);
		const Math::Vector3 axisZ = srt.rotate * Math::Vector3(0.0f, 0.0f, halfSize.z * srt.scale.z);
		const Math::Vector3 extent(
			std::fabs(axisX.x) + std::fabs(axisY.x) + std::fabs(axisZ.x),
			std::fabs(axisX.y) + std::fabs(axisY.y) + std::fabs(axisZ.y),
			std::fabs(axisX.z) + std::fabs(axisY.z) + std::fabs(axisZ.z));
		auto& aabb = std::get<Math::AABB>(shape_);
		aabb.min = center - extent;
		aabb.max = center + extent;
		aabb.center = center;
	} else if (std::holds_alternative<Math::OBB>(shape_)) {
		std::get<Math::OBB>(shape_).size = size_;
		std::get<Math::OBB>(shape_).MakeOBBAxis(srt.rotate);
		localSRT_.rotate = srt.rotate;
		Math::Matrix4x4 worldMat = localSRT_.MakeAffine();
		std::get<Math::OBB>(shape_).center = srt.translate + DecomposeTranslate(worldMat);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　描画処理
//////////////////////////////////////////////////////////////////////////////////////////////////
void BoxCollider::Draw() const {
	AOENGINE::Color color = Colors::Linear::white;
	if (collisionState_ == (int)CollisionFlags::Enter || collisionState_ == (int)CollisionFlags::Stay) {
		color = Colors::Linear::red;
	}

	if (std::holds_alternative<Math::AABB>(shape_)) {
		DrawAABB(std::get<Math::AABB>(shape_), AOENGINE::Render::GetViewProjectionMat(), color);
	} else if (std::holds_alternative<Math::OBB>(shape_)) {
		DrawOBB(std::get<Math::OBB>(shape_), AOENGINE::Render::GetViewProjectionMat(), color);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　編集処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void BoxCollider::Debug_Gui() {
	ImGui::DragFloat3("size", &size_.x);

	// ----------------------
	// ↓ shapeの表示
	// ----------------------
	if (std::holds_alternative<Math::AABB>(shape_)) {
		ImGui::Text("shape : AABB");
	} else if (std::holds_alternative<Math::OBB>(shape_)) {
		ImGui::Text("shape : OBB");
	}

	// ----------------------
	// ↓ shapeの変更
	// ----------------------
	int shapeTypeIndex = 0;
	std::string shapes[2] = { "AABB", "OBB" };
	if (ImGui::BeginCombo("shape", shapes[shapeTypeIndex].c_str())) {
		for (int i = 0; i < 2; ++i) {
			bool isSelected = (shapeTypeIndex == i);
			if (ImGui::Selectable(shapes[i].c_str(), isSelected)) {
				shapeTypeIndex = i;
				
				if (i == 0) {
					shape_ = Math::AABB{ .min = CVector3::UNIT * -1.0f, .max = CVector3::UNIT };
				} else {
					shape_ = Math::OBB{ .center = CVector3::ZERO, .size = CVector3::UNIT };
				}
			}
			if (isSelected) {
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}

	BaseCollider::Debug_Gui();
}

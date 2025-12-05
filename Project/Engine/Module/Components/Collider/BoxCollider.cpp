#include "BoxCollider.h"
#include "Engine/Utilities/DrawUtils.h"
#include "Engine/Render.h"
#include "Engine/System/Manager/CollisionLayerManager.h"
#include <assert.h>
#include <array>

BoxCollider::BoxCollider() {}
BoxCollider::~BoxCollider() {}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　初期化処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void BoxCollider::Init(const std::string& categoryName, ColliderShape shape) {
	auto& layers = CollisionLayerManager::GetInstance();
	categoryBits_ = layers.RegisterCategory(categoryName);
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
		// ローカル空間でのAABBの半サイズ
		Math::Vector3 halfSize = size_ * 0.5f;

		// ローカル空間での8頂点
		std::array<Math::Vector3, 8> localPoints = {
			Math::Vector3{-halfSize.x, -halfSize.y, -halfSize.z},
			Math::Vector3{ halfSize.x, -halfSize.y, -halfSize.z},
			Math::Vector3{-halfSize.x,  halfSize.y, -halfSize.z},
			Math::Vector3{ halfSize.x,  halfSize.y, -halfSize.z},
			Math::Vector3{-halfSize.x, -halfSize.y,  halfSize.z},
			Math::Vector3{ halfSize.x, -halfSize.y,  halfSize.z},
			Math::Vector3{-halfSize.x,  halfSize.y,  halfSize.z},
			Math::Vector3{ halfSize.x,  halfSize.y,  halfSize.z}
		};

		// 最大値と最小値を決定
		Math::Vector3 min = Math::Vector3{
			std::numeric_limits<float>::max(),
			std::numeric_limits<float>::max(),
			std::numeric_limits<float>::max()
		};
		Math::Vector3 max = Math::Vector3{
			std::numeric_limits<float>::lowest(),
			std::numeric_limits<float>::lowest(),
			std::numeric_limits<float>::lowest()
		};

		// aabbの各頂点を計算
		for (const auto& localPt : localPoints) {
			Math::Vector3 scaledPt = (localPt + localSRT_.translate) * srt.scale; // スケーリング
			Math::Vector3 rotatedPt = srt.rotate * scaledPt;                      // 回転
			Math::Vector3 worldPt = srt.translate + rotatedPt;                    // 平行移動
			min = Math::Vector3::Min(min, worldPt);
			max = Math::Vector3::Max(max, worldPt);
		}

		auto& aabb = std::get<Math::AABB>(shape_);
		aabb.min = min;
		aabb.max = max;
		aabb.center = (min + max) * 0.5f;
	} else if (std::holds_alternative<Math::OBB>(shape_)) {
		std::get<Math::OBB>(shape_).center = srt.translate + localSRT_.translate;
		std::get<Math::OBB>(shape_).MakeOBBAxis(srt.rotate);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　描画処理
//////////////////////////////////////////////////////////////////////////////////////////////////
void BoxCollider::Draw() const {
	Color color = Color::white;
	if (collisionState_ == (int)CollisionFlags::Enter || collisionState_ == (int)CollisionFlags::Stay) {
		color = Color::red;
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
}
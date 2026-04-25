#include "BaseCollider.h"
#include <assert.h>
#include "Engine/System/Manager/ImGuiManager.h"
#include "Engine/System/Manager/CollisionLayerManager.h"

using namespace AOENGINE;

BaseCollider::BaseCollider() {
	penetrationPrevention_ = false;	// 貫通対策を行うかどうか
	pushbackDire_ = CVector3::ZERO;
}

void AOENGINE::BaseCollider::Debug_Gui() {
	auto& layers = AOENGINE::CollisionLayerManager::GetInstance();

	// ----------------------
	// ↓ カテゴリの設定
	// ----------------------
	int currentIndex = 0;
	std::vector<std::string> categoties = layers.GetCategories();
	if (ImGui::BeginCombo("Category", categoties[currentIndex].c_str())) {
		for (int i = 0; i < categoties.size(); ++i) {
			bool isSelected = (currentIndex == i);
			if (ImGui::Selectable(categoties[i].c_str(), isSelected)) {
				currentIndex = i;
				layerBit_ = layers.RegisterCategory(categoties[i]);
				categoryName_ = categoties[i];
			}
			if (isSelected) {
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}

	// ----------------------
	// ↓ maskの設定
	// ----------------------
	if (ImGui::TreeNode("Collision Mask")) {
		const auto& categories = layers.GetCategories();

		for (const auto& categoryName : categories) {
			uint32_t bit = layers.GetCategoryBit(categoryName);

			bool enabled = (collisionMaskBit_ & bit) != 0;

			if (ImGui::Checkbox(categoryName.c_str(), &enabled)) {
				if (enabled) {
					collisionMaskBit_ |= bit;
				} else {
					collisionMaskBit_ &= ~bit;
				}
			}
		}

		ImGui::TreePop();
	}
}

void BaseCollider::SwitchCollision(BaseCollider* partner) {
	int& state = collisionPartnersMap_[partner];

	switch (state) {
	case 0b00:	// 衝突していない
		collisionState_ = (int)CollisionFlags::Enter;	// NONE → ENTER
		state = (int)CollisionFlags::Enter;
		break;
	case 0b01:	// 初衝突
		collisionState_ = (int)CollisionFlags::Stay;		// ENTER → STAY
		state = (int)CollisionFlags::Stay;
		break;
	case 0b10:	// 衝突しなくなった直後
		collisionState_ = (int)CollisionFlags::None;		// EXIT → NONE
		state = (int)CollisionFlags::None;
		break;
	case 0b11:	// 連続衝突時
		break;
	default:
		// 存在していない衝突判定
		assert("not registered CollisionState");
		break;
	}
}

void BaseCollider::DeletePartner(BaseCollider* partner) {
	collisionPartnersMap_.erase(partner);
}

void BaseCollider::SetTarget(const std::string& id) {
	auto& layers = AOENGINE::CollisionLayerManager::GetInstance();
	uint32_t bit = layers.GetCategoryBit(id);
	SetCollisionMaskBit(bit);
}

void BaseCollider::SetCategory(const std::string& category) {
	auto& layers = AOENGINE::CollisionLayerManager::GetInstance();
	layerBit_ = layers.RegisterCategory(category);
	categoryName_ = category;
}

void BaseCollider::SetPushBackDirection(const Math::Vector3& dire) {
	pushbackDire_ += dire;
}

void BaseCollider::OnCollision(BaseCollider* other) {
	if (onCollision_) {
		onCollision_(other);
	}
}



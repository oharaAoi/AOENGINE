#include "ICollider.h"
#include <assert.h>
#include "Engine/System/Manager/CollisionLayerManager.h"

void ICollider::SwitchCollision(ICollider* partner) {
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

void ICollider::DeletePartner(ICollider* partner) {
	collisionPartnersMap_.erase(partner);
}

void ICollider::SetTarget(const std::string& id) {
	auto& layers = CollisionLayerManager::GetInstance();
	uint32_t bit = layers.GetCategoryBit(id);
	SetMaskBits(bit);
}

void ICollider::SetCategory(const std::string& category) {
	auto& layers = CollisionLayerManager::GetInstance();
	categoryBits_ = layers.RegisterCategory(category);
	categoryName_ = category;
}

void ICollider::SetPushBackDirection(const Vector3& dire) {
	pushbackDire_ += dire;
}

void ICollider::OnCollision(ICollider* other) {
	if (onCollision_) {
		onCollision_(other);
	}
}



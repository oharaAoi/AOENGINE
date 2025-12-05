#include "BaseCollider.h"
#include <assert.h>
#include "Engine/System/Manager/CollisionLayerManager.h"

BaseCollider::BaseCollider() {
	penetrationPrevention_ = false;	// 貫通対策を行うかどうか
	pushbackDire_ = CVector3::ZERO;
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
	auto& layers = CollisionLayerManager::GetInstance();
	uint32_t bit = layers.GetCategoryBit(id);
	SetMaskBits(bit);
}

void BaseCollider::SetCategory(const std::string& category) {
	auto& layers = CollisionLayerManager::GetInstance();
	categoryBits_ = layers.RegisterCategory(category);
	categoryName_ = category;
}

void BaseCollider::SetPushBackDirection(const Vector3& dire) {
	pushbackDire_ += dire;
}

void BaseCollider::OnCollision(BaseCollider* other) {
	if (onCollision_) {
		onCollision_(other);
	}
}



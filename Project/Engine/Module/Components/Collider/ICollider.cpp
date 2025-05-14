#include "ICollider.h"
#include <assert.h>
#include "Engine/System/Manager/CollisionLayerManager.h"

#ifdef _DEBUG
void ICollider::Debug_Gui() {
	ImGui::DragFloat3("translate", &localSRT_.translate.x, 0.1f);
}
#endif

void ICollider::SwitchCollision(ICollider* partner) {
	int& state = collisionPartnersMap_[partner];

	switch (state) {
	case 0b00:	// 衝突していない
		collisionState_ = CollisionFlags::ENTER;	// NONE → ENTER
		state = CollisionFlags::ENTER;
		break;
	case 0b01:	// 初衝突
		collisionState_ = CollisionFlags::STAY;		// ENTER → STAY
		state = CollisionFlags::STAY;
		break;
	case 0b10:	// 衝突しなくなった直後
		collisionState_ = CollisionFlags::NONE;		// EXIT → NONE
		state = CollisionFlags::NONE;
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

void ICollider::SetPushBackDirection(const Vector3& dire) {
	if (pushbackDire_.x > dire.x) {
		pushbackDire_.x = dire.x;
	}

	if (pushbackDire_.y > dire.y) {
		pushbackDire_.y = dire.y;
	}

	if (pushbackDire_.z > dire.z) {
		pushbackDire_.z = dire.z;
	}
}



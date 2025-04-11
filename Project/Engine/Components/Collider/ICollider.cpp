#include "ICollider.h"
#include <assert.h>

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
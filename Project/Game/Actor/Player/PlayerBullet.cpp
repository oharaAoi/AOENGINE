#include "PlayerBullet.h"

PlayerBullet::~PlayerBullet() {
	BaseBullet::Finalize();
}

void PlayerBullet::Init() {
	BaseBullet::Init();
	SetObject("playerBullet.obj");
}

void PlayerBullet::Update() {
	if (std::abs(transform_->translate_.x) >= 200.0f) {
		isAlive_ = false;
	}

	if (std::abs(transform_->translate_.y) >= 200.0f) {
		isAlive_ = false;
	}

	if (std::abs(transform_->translate_.z) >= 10.0f) {
		isAlive_ = false;
	}

	BaseBullet::Update();
}

void PlayerBullet::Draw() const {
	BaseBullet::Draw();
}

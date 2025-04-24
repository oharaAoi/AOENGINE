#include "GunFireParticles.h"

void GunFireParticles::Init(const std::string& name) {
	BaseParticles::Init(name);
}

void GunFireParticles::Update(const Quaternion& bill) {
	BaseParticles::Update(bill);
}

void GunFireParticles::SetParent(const Matrix4x4& parentMat) {
	BaseParticles::SetParent(parentMat);
}

void GunFireParticles::SetOnShot() {
	emitter_.isOneShot = true;
}

void GunFireParticles::SetPos(const Vector3& pos) {
	emitter_.translate = pos;
}

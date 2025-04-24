#include "HitBossSmoke.h"

void HitBossSmoke::Init(const std::string& name) {
	BaseParticles::Init(name, false);
}

void HitBossSmoke::Update(const Quaternion& bill) {
	BaseParticles::Update(bill);
}

void HitBossSmoke::SetParent(const Matrix4x4& parentMat) {
	BaseParticles::SetParent(parentMat);
}

void HitBossSmoke::SetOnShot() {
	emitter_.isOneShot = true;
}

void HitBossSmoke::SetPos(const Vector3& pos) {
	emitter_.translate = pos;
}

#include "HitBossSmokeBorn.h"

void HitBossSmokeBorn::Init(const std::string& name) {
	BaseParticles::Init(name, false);
}

void HitBossSmokeBorn::Update(const Quaternion& bill) {
	BaseParticles::Update(bill);
}

void HitBossSmokeBorn::SetParent(const Matrix4x4& parentMat) {
	BaseParticles::SetParent(parentMat);
}

void HitBossSmokeBorn::SetOnShot() {
	emitter_.isOneShot = true;
}

void HitBossSmokeBorn::SetPos(const Vector3& pos) {
	emitter_.translate = pos;
}

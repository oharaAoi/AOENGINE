#include "HitBossExploadParticles.h"

void HitBossExploadParticles::Init(const std::string& name) {
	BaseParticles::Init(name);
}

void HitBossExploadParticles::Update(const Quaternion& bill) {
	BaseParticles::Update(bill);
}

void HitBossExploadParticles::SetParent(const Matrix4x4& parentMat) {
	BaseParticles::SetParent(parentMat);
}

void HitBossExploadParticles::SetOnShot() {
	emitter_.isOneShot = true;
}

void HitBossExploadParticles::SetPos(const Vector3& pos) {
	emitter_.translate = pos;
}


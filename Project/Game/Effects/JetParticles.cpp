#include "JetParticles.h"

void JetParticles::Init(const std::string& name) {
	BaseParticles::Init(name);
}

void JetParticles::Update(const Quaternion& bill) {
	BaseParticles::Update(bill);
}

void JetParticles::SetParent(const Matrix4x4& parentMat) {
	BaseParticles::SetParent(parentMat);
}


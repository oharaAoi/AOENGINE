#include "JetEnergyParticles.h"

void JetEnergyParticles::Init(const std::string& name) {
	BaseParticles::Init(name);
}

void JetEnergyParticles::Update(const Quaternion& bill) {
	BaseParticles::Update(bill);
}

void JetEnergyParticles::SetParent(const Matrix4x4& parentMat) {
	BaseParticles::SetParent(parentMat);
}

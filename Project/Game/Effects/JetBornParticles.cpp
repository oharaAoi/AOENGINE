#include "JetBornParticles.h"

void JetBornParticles::Init(const std::string& name) {
	BaseParticles::Init(name);
}

void JetBornParticles::Update(const Quaternion& bill) {
	BaseParticles::Update(bill);
}

void JetBornParticles::SetParent(const Matrix4x4& parentMat) {
	BaseParticles::SetParent(parentMat);
}

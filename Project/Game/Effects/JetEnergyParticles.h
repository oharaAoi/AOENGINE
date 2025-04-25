#pragma once
#include "Game/Effects/BaseParticles.h"

class JetEnergyParticles 
	: BaseParticles {
public:
	JetEnergyParticles() = default;
	~JetEnergyParticles() override {};

	void Init(const std::string& name);

	void Update(const Quaternion& bill);

	void SetParent(const Matrix4x4& parentMat) override;
};


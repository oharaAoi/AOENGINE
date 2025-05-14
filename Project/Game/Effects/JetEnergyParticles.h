#pragma once
#include "Engine/Module/Components/Effect/BaseParticles.h"

class JetEnergyParticles :
	public BaseParticles {
public:
	JetEnergyParticles() = default;
	~JetEnergyParticles() override {};

	void Init(const std::string& name);

	void Update(const Quaternion& bill);

	void SetParent(const Matrix4x4& parentMat) override;
};


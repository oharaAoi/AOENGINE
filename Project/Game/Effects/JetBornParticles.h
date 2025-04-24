#pragma once
#include "Game/Effects/BaseParticles.h"

class JetBornParticles
	: BaseParticles {
public:

	JetBornParticles() = default;
	~JetBornParticles() override {};

	void Init(const std::string& name);

	void Update(const Quaternion& bill);

	void SetParent(const Matrix4x4& parentMat) override;

private:


	
};


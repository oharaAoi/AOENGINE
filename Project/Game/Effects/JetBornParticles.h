#pragma once
#include "Engine/Module/Components/Effect/BaseParticles.h"

class JetBornParticles :
	public BaseParticles {
public:

	JetBornParticles() = default;
	~JetBornParticles() override {};

	void Init(const std::string& name);

	void Update(const Quaternion& bill);

	void SetParent(const Matrix4x4& parentMat) override;

private:


	
};


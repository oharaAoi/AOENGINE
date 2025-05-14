#pragma once
#include "Engine/Module/Components/Effect/BaseParticles.h"

class JetParticles :
	public BaseParticles{
public:

	JetParticles() = default;
	~JetParticles() override {};

	void Init(const std::string& name);

	void Update(const Quaternion& bill);

	void SetParent(const Matrix4x4& parentMat) override;

};


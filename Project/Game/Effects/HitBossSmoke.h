#pragma once
#include "Engine/Module/Components/Effect/BaseParticles.h"

class HitBossSmoke :
	public BaseParticles {
public:

	HitBossSmoke() = default;
	~HitBossSmoke() override {};

	void Init(const std::string& name);

	void Update(const Quaternion& bill);

	void SetParent(const Matrix4x4& parentMat) override;

	void SetOnShot();

	void SetPos(const Vector3& pos);
};


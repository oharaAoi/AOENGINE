#pragma once
#include "Game/Effects/BaseParticles.h"

class HitBossSmoke
	: BaseParticles {
public:

	HitBossSmoke() = default;
	~HitBossSmoke() override {};

	void Init(const std::string& name);

	void Update(const Quaternion& bill);

	void SetParent(const Matrix4x4& parentMat) override;

	void SetOnShot();

	void SetPos(const Vector3& pos);
};


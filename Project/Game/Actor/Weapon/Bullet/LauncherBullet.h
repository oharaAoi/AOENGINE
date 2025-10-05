#pragma once
#include "Game/Actor/Base/BaseBullet.h"
#include "Engine/Module/Components/Effect/BaseParticles.h"

class LauncherBullet :
	public BaseBullet {
public:

	LauncherBullet() = default;
	~LauncherBullet() override;

	void Init();

	void Update() override;

	void OnCollision(ICollider* other);

	void Reset(const Vector3& _pos, const Vector3& _velocity);

private:

	void Tracking();

private:
	BaseParticles* burn_;
	BaseParticles* smoke_;

};

